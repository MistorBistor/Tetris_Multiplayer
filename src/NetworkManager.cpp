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

    // Bind listener na dowolnym dostępnym porcie
    if (listener.listen(tcpPort) != sf::Socket::Done) {
        std::cout << "[NetworkManager ERROR] Nie można nasłuchiwać na porcie "
            << tcpPort << "\n";
        return false;
    }

    // Ustaw non-blocking żeby nie blokować głównej pętli
    listener.setBlocking(false);

    std::cout << "[NetworkManager] Lobby stworzone na porcie " << tcpPort
        << "\n";

    // Bind UDP socket do broadcastowania
    if (udpSocket.bind(BROADCAST_PORT) != sf::Socket::Done) {
        std::cout << "[NetworkManager ERROR] Nie można zbindować UDP\n";
        return false;
    }

    udpSocket.setBlocking(false);

    return true;
}

bool NetworkManager::listenForClient() {
    if (!isHost || connected) {
        return false;
    }

    // Sprawdź czy ktoś się łączy
    if (listener.accept(playerSocket) == sf::Socket::Done) {
        std::cout << "[NetworkManager] Klient się połączył: "
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

    std::cout << "[NetworkManager] Wysyłam sygnał START GAME\n";

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
        std::cout << "[NetworkManager ERROR] Nie można zbindować search socket\n";
        return lobbies;
    }

    searchSocket.setBlocking(false);

    // Wyślij request broadcast
    sf::Packet requestPacket;
    requestPacket << static_cast<uint8_t>(MessageType::LOBBY_LIST_REQUEST);

    // DODAJ DEBUG ↓↓↓
    std::cout << "[NetworkManager] Mój IP: " << sf::IpAddress::getLocalAddress() << "\n";
    std::cout << "[NetworkManager] Port wyszukiwania: " << searchSocket.getLocalPort() << "\n";
    // ↑↑↑ KONIEC DEBUG

    // Broadcast do całej sieci lokalnej
    if (searchSocket.send(requestPacket, sf::IpAddress::Broadcast, BROADCAST_PORT) != sf::Socket::Done) {
        std::cout << "[NetworkManager ERROR] Nie udało się wysłać broadcastu\n";
        return lobbies;
    }

    std::cout << "[NetworkManager] Wysłano broadcast request na port " << BROADCAST_PORT << "\n";

    // Czekaj na odpowiedzi przez timeoutSeconds
    sf::Clock clock;
    while (clock.getElapsedTime().asSeconds() < timeoutSeconds) {
        sf::Packet responsePacket;
        sf::IpAddress senderIP;
        unsigned short senderPort;

        sf::Socket::Status status = searchSocket.receive(responsePacket, senderIP, senderPort);

        // DODAJ DEBUG ↓↓↓
        if (status == sf::Socket::Error) {
            std::cout << "[NetworkManager] Błąd odbierania pakietu\n";
        }
        else if (status == sf::Socket::NotReady) {
            // Normalnie, nic nie przyszło jeszcze
        }
        else if (status == sf::Socket::Disconnected) {
            std::cout << "[NetworkManager] Socket disconnected\n";
        }
        // ↑↑↑ KONIEC DEBUG

        if (status == sf::Socket::Done) {
            uint8_t msgType;
            responsePacket >> msgType;

            std::cout << "[NetworkManager] Otrzymano pakiet od " << senderIP
                << ":" << senderPort << " typu " << (int)msgType << "\n";

            if (static_cast<MessageType>(msgType) == MessageType::LOBBY_ANNOUNCEMENT) {
                // Odebrano ogłoszenie lobby
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

        // Małe opóźnienie żeby nie zjadać CPU
        sf::sleep(sf::milliseconds(50));
    }

    std::cout << "[NetworkManager] Znaleziono " << lobbies.size() << " lobby\n";
    return lobbies;
}

bool NetworkManager::joinLobby(const LobbyInfo& lobby) {
    std::cout << "[NetworkManager] Łączenie z lobby: " << lobby.name << " ("
        << lobby.hostIP << ":" << lobby.hostPort << ")\n";

    isHost = false;
    opponentIP = sf::IpAddress(lobby.hostIP);

    // Połącz się z hostem
    if (playerSocket.connect(opponentIP, lobby.hostPort, sf::seconds(5)) !=
        sf::Socket::Done) {
        std::cout << "[NetworkManager ERROR] Nie można połączyć się z hostem\n";
        return false;
    }

    playerSocket.setBlocking(false);
    connected = true;

    std::cout << "[NetworkManager] Połączono z hostem!\n";
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

    // Wyślij rozmiar planszy
    packet << static_cast<uint32_t>(boardData.size());
    packet << static_cast<uint32_t>(boardData[0].size());

    // Wyślij dane planszy
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

    std::cout << "[NetworkManager] Wysyłam atak: " << lineCount << " linii\n";

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

    std::cout << "[NetworkManager] Wysyłam GAME OVER\n";

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
        std::cout << "[NetworkManager] Przeciwnik przegrał!\n";
        return true;
    }

    return false;
}

void NetworkManager::disconnect() {
    if (connected) {
        std::cout << "[NetworkManager] Rozłączanie...\n";

        // Wyślij informację o rozłączeniu
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
        std::cout << "[NetworkManager ERROR] Błąd wysyłania pakietu\n";
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
        std::cout << "[NetworkManager] Przeciwnik się rozłączył\n";
        connected = false;
    }

    return false;
}
void NetworkManager::respondToBroadcastRequests() {
    if (!isHost) {
        return;
    }

    // Sprawdź czy ktoś wysłał request o listę lobby
    sf::Packet requestPacket;
    sf::IpAddress senderIP;
    unsigned short senderPort;

    if (udpSocket.receive(requestPacket, senderIP, senderPort) == sf::Socket::Done) {
        uint8_t msgType;
        requestPacket >> msgType;

        if (static_cast<MessageType>(msgType) == MessageType::LOBBY_LIST_REQUEST) {
            std::cout << "[NetworkManager] Otrzymano request od " << senderIP << "\n";

            // Wyślij informację o naszym lobby
            sf::Packet responsePacket;
            responsePacket << static_cast<uint8_t>(MessageType::LOBBY_ANNOUNCEMENT);
            responsePacket << lobbyName;
            responsePacket << tcpPort;
            responsePacket << getPlayerCount();

            // Wyślij odpowiedź do requestera
            if (udpSocket.send(responsePacket, senderIP, senderPort) == sf::Socket::Done) {
                std::cout << "[NetworkManager] Wysłano info o lobby do " << senderIP << "\n";
            }
            else {
                std::cout << "[NetworkManager ERROR] Nie udało się wysłać odpowiedzi\n";
            }
        }
    }
}