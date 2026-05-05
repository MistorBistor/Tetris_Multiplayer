#pragma once
#include <SFML/Network.hpp>
#include <string>
#include <vector>

// Typy wiadomoœci sieciowych
enum class MessageType : uint8_t {
    LOBBY_ANNOUNCEMENT = 0,  // Host og³asza swoje lobby
    LOBBY_LIST_REQUEST = 1,  // Klient prosi o listê lobby
    LOBBY_LIST_RESPONSE = 2, // OdpowiedŸ z list¹ lobby
    JOIN_REQUEST = 3,        // Klient chce do³¹czyæ
    JOIN_RESPONSE = 4,       // Host odpowiada (akceptacja/odrzucenie)
    GAME_START = 5,          // Host rozpoczyna grê
    BOARD_STATE = 6,         // Synchronizacja stanu planszy
    LINES_CLEARED = 7,       // Gracz usun¹³ linie (atak)
    GAME_OVER = 8,           // Gracz przegra³
    DISCONNECT = 9           // Roz³¹czenie
};

// Struktura informacji o lobby
struct LobbyInfo {
    std::string name;        // Nazwa lobby
    std::string hostIP;      // IP hosta
    unsigned short hostPort; // Port hosta
    int playerCount;         // Liczba graczy (1 lub 2)

    LobbyInfo() : name(""), hostIP(""), hostPort(0), playerCount(0) {}
    LobbyInfo(std::string n, std::string ip, unsigned short p, int count)
        : name(n), hostIP(ip), hostPort(p), playerCount(count) {
    }
};

// Klasa zarz¹dzaj¹ca po³¹czeniem sieciowym
class NetworkManager {
private:
    // Tryb: true = host, false = client
    bool isHost;

    // Nazwa lobby (tylko dla hosta)
    std::string lobbyName;

    // Socket TCP dla po³¹czenia z drugim graczem
    sf::TcpSocket playerSocket;

    // Listener dla hosta (nas³uchuje po³¹czeñ)
    sf::TcpListener listener;

    // Socket UDP do broadcastowania/odbierania informacji o lobby
    sf::UdpSocket udpSocket;

    // Port UDP dla discovery (broadcast)
    static const unsigned short BROADCAST_PORT = 54001;

    // Port TCP dla po³¹czenia gry
    unsigned short tcpPort;

    // Czy jesteœmy po³¹czeni z drugim graczem
    bool connected;

    // IP przeciwnika (dla klienta)
    sf::IpAddress opponentIP;

    // Pomocnicza funkcja do wysy³ania/odbierania pakietów
    bool sendPacket(sf::Packet& packet);

public:
    NetworkManager();
    ~NetworkManager();

    // === FUNKCJE HOSTA ===

    // Stwórz lobby jako host
    // Zwraca true jeœli uda³o siê
    bool createLobby(const std::string& name);

    // Nas³uchuj na po³¹czenia od klientów (non-blocking)
    // Zwraca true jeœli ktoœ siê po³¹czy³
    bool listenForClient();

    // Rozpocznij grê (wyœlij sygna³ do klienta)
    void startGame();

    // === FUNKCJE KLIENTA ===

    // Wyszukaj dostêpne lobby w sieci lokalnej
    // Zwraca listê znalezionych lobby
    std::vector<LobbyInfo> searchLobbies(float timeoutSeconds = 2.0f);

    // Do³¹cz do lobby
    bool joinLobby(const LobbyInfo& lobby);

    // === FUNKCJE WSPÓLNE ===

    // Wyœlij stan planszy do przeciwnika
    void sendBoardState(const std::vector<std::vector<int>>& boardData);

    // Odbierz stan planszy od przeciwnika (non-blocking)
    // Zwraca true jeœli otrzymano nowe dane
    bool receiveBoardState(std::vector<std::vector<int>>& outBoardData);

    // Wyœlij informacjê o usuniêtych liniach (atak)
    void sendLinesCleared(int lineCount);

    // Odbierz informacjê o ataku (non-blocking)
    // Zwraca liczbê linii do dodania (0 jeœli nic nie otrzymano)
    int receiveAttack();

    // Wyœlij Game Over
    void sendGameOver();

    // SprawdŸ czy przeciwnik przegra³ (non-blocking)
    bool receivedOpponentGameOver();

    // Roz³¹cz siê
    void disconnect();

    // Gettery
    bool isConnected() const { return connected; }
    bool isHosting() const { return isHost; }
    std::string getLobbyName() const { return lobbyName; }
    int getPlayerCount() const { return connected ? 2 : 1; }

    // Pomocnicza funkcja do wysy³ania/odbierania pakietów
    bool receivePacket(sf::Packet& packet);

    // Host odpowiada na broadcast requesty
    void respondToBroadcastRequests();
};