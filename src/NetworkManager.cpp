#include "NetworkManager.h"
#include <iostream>
#include <thread>

NetworkManager::NetworkManager()
    : isHost(false), tcpPort(54000), connected(false) {
    std::cout << "[NetworkManager] Inicjalizacja\n";
}

NetworkManager::~NetworkManager() { disconnect(); }

// === HOST ===

bool NetworkManager::createLobby(const std::string& name) {
    std::cout << "[NetworkManager] Tworzenie lobby: " << name << "\n";

    isHost = true;
    lobbyName = name;
    connected = false;

    // Bind listener na dowolnym dostêpnym porcie
    if (listener.listen(tcpPort) != sf::Socket::Done) {
        std::cout << "[NetworkManager ERROR] Nie mo¿na nas³uchiwaæ na porcie "
            << tcpPort << "\n";
        return false;
    }

    // Ustaw non-blocking ¿eby nie blokowaæ g³ównej pêtli
    listener.setBlocking(false);

    std::cout << "[NetworkManager] Lobby stworzone na porcie " << tcpPort
        << "\n";

    // Bind UDP socket do broadcastowania
    if (udpSocket.bind(BROADCAST_PORT) != sf::Socket::Done) {
        std::cout << "[NetworkManager ERROR] Nie mo¿na zbindowaæ UDP\n";
        return false;
    }

    udpSocket.setBlocking(false);

    return true;
}

bool NetworkManager::listenForClient() {
    if (!isHost || connected) {
        return false;
    }

    // SprawdŸ czy ktoœ siê ³¹czy
    if (listener.accept(playerSocket) == sf::Socket::Done) {
        std::cout << "[NetworkManager] Klient siê po³¹czy³: "
            << playerSocket.getRemoteAddress() << "\n";

        playerSocket.setBlocking(false);
        connected = true;
        return true;
    }

    return false;
}

void NetworkManager::startGame() {
    if (!isHost || !connected) {
        return;
    }

    std::cout << "[NetworkManager] Wysy³am sygna³ START GAME\n";

    sf::Packet packet;
    packet << static_cast<uint8_t>(MessageType::GAME_START);

    sendPacket(packet);
}

// === CLIENT ===

std::vector<LobbyInfo> NetworkManager::searchLobbies(float timeoutSeconds) {
    std::cout << "[NetworkManager] Szukam lobby...\n";

    std::vector<LobbyInfo> lobbies;

    // Bind UDP socket do odbierania broadcastów
    sf::UdpSocket searchSocket;
    if (searchSocket.bind(sf::Socket::AnyPort) != sf::Socket::Done) {
        std::cout << "[NetworkManager ERROR] Nie mo¿na zbindowaæ search socket\n";
        return lobbies;
    }

    searchSocket.setBlocking(false);

    // Wyœlij request broadcast
    sf::Packet requestPacket;
    requestPacket << static_cast<uint8_t>(MessageType::LOBBY_LIST_REQUEST);

    // Broadcast do ca³ej sieci lokalnej
    searchSocket.send(requestPacket, sf::IpAddress::Broadcast, BROADCAST_PORT);

    std::cout << "[NetworkManager] Wys³ano broadcast request\n";

    // Czekaj na odpowiedzi przez timeoutSeconds
    sf::Clock clock;
    while (clock.getElapsedTime().asSeconds() < timeoutSeconds) {
        sf::Packet responsePacket;
        sf::IpAddress senderIP;
        unsigned short senderPort;

        if (searchSocket.receive(responsePacket, senderIP, senderPort) ==
            sf::Socket::Done) {
            uint8_t msgType;
            responsePacket >> msgType;

            if (static_cast<MessageType>(msgType) ==
                MessageType::LOBBY_ANNOUNCEMENT) {
                // Odebrano og³oszenie lobby
                std::string name;
                unsigned short tcpPort;
                int playerCount;

                responsePacket >> name >> tcpPort >> playerCount;

                std::cout << "[NetworkManager] Znaleziono lobby: " << name << " ("
                    << senderIP << ":" << tcpPort << ") - " << playerCount
                    << "/2 graczy\n";

                lobbies.push_back(LobbyInfo(name, senderIP.toString(), tcpPort,
                    playerCount));
            }
        }

        // Ma³e opóŸnienie ¿eby nie zjadaæ CPU
        sf::sleep(sf::milliseconds(50));
    }

    std::cout << "[NetworkManager] Znaleziono " << lobbies.size() << " lobby\n";
    return lobbies;
}

bool NetworkManager::joinLobby(const LobbyInfo& lobby) {
    std::cout << "[NetworkManager] £¹czenie z lobby: " << lobby.name << " ("
        << lobby.hostIP << ":" << lobby.hostPort << ")\n";

    isHost = false;
    opponentIP = sf::IpAddress(lobby.hostIP);

    // Po³¹cz siê z hostem
    if (playerSocket.connect(opponentIP, lobby.hostPort, sf::seconds(5)) !=
        sf::Socket::Done) {
        std::cout << "[NetworkManager ERROR] Nie mo¿na po³¹czyæ siê z hostem\n";
        return false;
    }

    playerSocket.setBlocking(false);
    connected = true;

    std::cout << "[NetworkManager] Po³¹czono z hostem!\n";
    return true;
}

// === WSPÓLNE ===

void NetworkManager::sendBoardState(
    const std::vector<std::vector<int>>& boardData) {
    if (!connected) {
        return;
    }

    sf::Packet packet;
    packet << static_cast<uint8_t>(MessageType::BOARD_STATE);

    // Wyœlij rozmiar planszy
    packet << static_cast<uint32_t>(boardData.size());
    packet << static_cast<uint32_t>(boardData[0].size());

    // Wyœlij dane planszy
    for (const auto& row : boardData) {
        for (int cell : row) {
            packet << static_cast<int32_t>(cell);
        }
    }

    sendPacket(packet);
}

bool NetworkManager::receiveBoardState(
    std::vector<std::vector<int>>& outBoardData) {
    if (!connected) {
        return false;
    }

    sf::Packet packet;
    if (!receivePacket(packet)) {
        return false;
    }

    uint8_t msgType;
    packet >> msgType;

    if (static_cast<MessageType>(msgType) != MessageType::BOARD_STATE) {
        return false;
    }

    // Odbierz rozmiar
    uint32_t rows, cols;
    packet >> rows >> cols;

    // Odbierz dane
    outBoardData.clear();
    outBoardData.resize(rows, std::vector<int>(cols));

    for (uint32_t r = 0; r < rows; r++) {
        for (uint32_t c = 0; c < cols; c++) {
            int32_t cell;
            packet >> cell;
            outBoardData[r][c] = cell;
        }
    }

    return true;
}

void NetworkManager::sendLinesCleared(int lineCount) {
    if (!connected || lineCount <= 0) {
        return;
    }

    std::cout << "[NetworkManager] Wysy³am atak: " << lineCount << " linii\n";

    sf::Packet packet;
    packet << static_cast<uint8_t>(MessageType::LINES_CLEARED);
    packet << static_cast<int32_t>(lineCount);

    sendPacket(packet);
}

int NetworkManager::receiveAttack() {
    if (!connected) {
        return 0;
    }

    sf::Packet packet;
    if (!receivePacket(packet)) {
        return 0;
    }

    uint8_t msgType;
    packet >> msgType;

    if (static_cast<MessageType>(msgType) != MessageType::LINES_CLEARED) {
        return 0;
    }

    int32_t lineCount;
    packet >> lineCount;

    std::cout << "[NetworkManager] Otrzymano atak: " << lineCount << " linii\n";

    return lineCount;
}

void NetworkManager::sendGameOver() {
    if (!connected) {
        return;
    }

    std::cout << "[NetworkManager] Wysy³am GAME OVER\n";

    sf::Packet packet;
    packet << static_cast<uint8_t>(MessageType::GAME_OVER);

    sendPacket(packet);
}

bool NetworkManager::receivedOpponentGameOver() {
    if (!connected) {
        return false;
    }

    sf::Packet packet;
    if (!receivePacket(packet)) {
        return false;
    }

    uint8_t msgType;
    packet >> msgType;

    if (static_cast<MessageType>(msgType) == MessageType::GAME_OVER) {
        std::cout << "[NetworkManager] Przeciwnik przegra³!\n";
        return true;
    }

    return false;
}

void NetworkManager::disconnect() {
    if (connected) {
        std::cout << "[NetworkManager] Roz³¹czanie...\n";

        // Wyœlij informacjê o roz³¹czeniu
        sf::Packet packet;
        packet << static_cast<uint8_t>(MessageType::DISCONNECT);
        sendPacket(packet);

        playerSocket.disconnect();
        connected = false;
    }

    if (isHost) {
        listener.close();
    }

    udpSocket.unbind();
}

// === PRYWATNE ===

bool NetworkManager::sendPacket(sf::Packet& packet) {
    if (playerSocket.send(packet) != sf::Socket::Done) {
        std::cout << "[NetworkManager ERROR] B³¹d wysy³ania pakietu\n";
        return false;
    }
    return true;
}

bool NetworkManager::receivePacket(sf::Packet& packet) {
    sf::Socket::Status status = playerSocket.receive(packet);

    if (status == sf::Socket::Done) {
        return true;
    }
    else if (status == sf::Socket::Disconnected) {
        std::cout << "[NetworkManager] Przeciwnik siê roz³¹czy³\n";
        connected = false;
    }

    return false;
}