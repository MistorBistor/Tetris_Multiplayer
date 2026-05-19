#ifndef MENU_HPP
#define MENU_HPP

#include "ColorTheme.h"
#include "NetworkManager.h"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

/**
 * @enum MenuState
 * @brief Stany menu
 */
enum class MenuState {
    MAIN_MENU,
    HIGH_SCORES,
    GAME,
    PAUSE,
    GAME_OVER,
    SETTINGS,
    DIFFICULTY_SELECTION,
    MULTIPLAYER_MENU,      // Menu wyboru Host/Join
    MULTIPLAYER_HOST,      // Ekran tworzenia lobby
    MULTIPLAYER_LOBBY,     // Ekran oczekiwania na graczy
    MULTIPLAYER_JOIN,       // Lista dostępnych lobby
    MULTIPLAYER_PAUSE
};
/**
 * @enum MenuAction
 * @brief Akcje zwracane przez menu po wyborze opcji
 */
enum class MenuAction {
    NONE = 0,
    START_GAME,
    CONTINUE,
    CONFIRM_DIFFICULTY,
    SETTINGS,
    HIGH_SCORES,
    EXIT,
    RESUME,
    RESTART,
    MAIN_MENU,
    CHANGE_THEME,
    MULTIPLAYER,           // NOWY: Przejdź do menu multiplayer
    HOST_GAME,             // NOWY: Stwórz lobby
    JOIN_GAME,             // NOWY: Szukaj lobby
    START_MULTIPLAYER,     // NOWY: Host startuje grę
    QUIT_LOBBY,            // NOWY: Wyjdź z lobby
    JOIN_SELECTED_LOBBY    // NOWY: Dołącz do wybranego lobby
};
/**
 * @class Menu
 * @brief System menu gry
 *
 * Klasa zarządzająca menu głównym, menu pauzy i ekranem końca gry.
 */
class Menu {
private:
  MenuState currentState;
  std::vector<std::string> menuItems;
  int selectedIndex;

  sf::Font font;
  sf::Text titleText;
  std::vector<sf::Text> itemTexts;

  int selectedDifficulty;
  int selectedDifficultyElement;

  static constexpr int MIN_DIFFICULTY = 0;
  static constexpr int MAX_DIFFICULTY = 9;

  sf::Text difficultyText;
  sf::Text leftArrowText;
  sf::Text rightArrowText;
  sf::Text confirmButton;
  sf::Text backButton;

  static const int WINDOW_WIDTH = 800;
  static const int WINDOW_HEIGHT = 850; // Zwiększone dla wyższego okna
  static const int TITLE_SIZE = 60;
  static const int ITEM_SIZE = 30;
  static const int ITEM_SPACING = 50;

  void updateDifficultyDisplay();

  std::vector<std::pair<std::string, int>> highScores;

  int selectedTheme = 0; // 0 = Classic, 1 = Dark, 2 = Neon

  ColorTheme currentTheme;

  // Do obsługi multiplayer
  std::string lobbyNameInput;        // Wpisywana nazwa lobby
  bool isTypingLobbyName;            // Czy gracz wpisuje nazwę
  std::vector<LobbyInfo> availableLobbies;  // Lista znalezionych lobby
  int selectedLobbyIndex;            // Który lobby jest wybrany
  int lobbyUISelectedElement;        // 0=input, 1=create, 2=back (dla host screen)

public:
  /**
   * @brief Konstruktor
   */
  Menu();

  /**
   * @brief Destruktor
   */
  ~Menu();

  /**
   * @brief Inicjalizacja menu
   */
  void initialize();

  /**
   * @brief Ustawienie stanu menu
   * @param state Nowy stan menu
   */
  void setState(MenuState state);

  /**
   * @brief Pobierz aktualny stan
   */
  MenuState getState() const { return currentState; }

  /**
   * @brief Przesunięcie wyboru w górę
   */
  void moveUp();

  /**
   * @brief Przesunięcie wyboru w dół
   */
  void moveDown();

  /**
   * @brief Pobierz wybrany indeks opcji
   */
  int getSelectedIndex() const { return selectedIndex; }

  /**
   * @brief Obsługa wyboru opcji menu
   * @return Akcja do wykonania (jako enum lub int)
   */
  MenuAction handleSelection();

  /**
   * @brief Obsługa zdarzeń menu
   * @param event Zdarzenie SFML
   */
  void handleEvent(const sf::Event &event);

  // Zwiększa wybrany poziom trudności
  void increaseDifficulty();

  // Zmniejsza wybrany poziom trudności
  void decreaseDifficulty();

  // Zwraca wybrany poziom trudności
  int getSelectedDifficulty() const { return selectedDifficulty; }

  // Konwertuje poziom trudności na prędkość spadania
  float getDifficultySpeed() const;

  // Sprawdza czy kliknięto w przyciski na ekranie wyboru trudności
  // Zwraca: 0 = nic, 1 = Confirm, 2 = Back
  int checkDifficultyClick(float mouseX, float mouseY) const;

  /**
   * @brief Renderowanie menu
   * @param window Okno do rysowania
   */
  void render(sf::RenderWindow &window) const;

  /**
   * @brief Wyświetlenie ekranu Game Over z wynikiem
   * @param score Końcowy wynik
   * @param isHighScore Czy to nowy rekord
   */
  void showGameOver(int score, bool isHighScore);

  const sf::Font &getFont() const { return font; }

  void setHighScores(const std::vector<std::pair<std::string, int>> &scores) {
    highScores = scores;
  }

  int getSelectedTheme() const { return selectedTheme; }

  void setTheme(const ColorTheme &theme);

  // Obsługa multiplayer
  void setAvailableLobbies(const std::vector<LobbyInfo>& lobbies);
  int getSelectedLobbyIndex() const { return selectedLobbyIndex; }
  std::string getLobbyNameInput() const { return lobbyNameInput; }
  void setTypingLobbyName(bool typing) { isTypingLobbyName = typing; }
  bool isTypingName() const { return isTypingLobbyName; }
  void addCharToLobbyName(char c);
  void removeCharFromLobbyName();
  void clearLobbyNameInput() { lobbyNameInput = ""; }

  // Renderowanie specjalnych ekranów multiplayer
  void renderMultiplayerHost(sf::RenderWindow& window) const;
  void renderMultiplayerLobby(sf::RenderWindow& window, int playerCount, const std::string& lobbyName) const;
  void renderMultiplayerJoin(sf::RenderWindow& window) const;

  const std::vector<LobbyInfo>& getAvailableLobbies() const { return availableLobbies; }

  // rendering dla multiplayer pause
  void renderMultiplayerPause(sf::RenderWindow& window);
};

#endif // MENU_HPP
