#ifndef MENU_HPP
#define MENU_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

/**
 * @enum MenuState
 * @brief Stany menu
 */
enum class MenuState { MAIN_MENU, GAME, PAUSE, GAME_OVER, SETTINGS, DIFFICULTY_SELECTION };
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
  MAIN_MENU
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
  std::vector < sf::Text> itemTexts;

  int selectedDifficulty;
  int selectedDifficultyElement;

  static constexpr int MIN_DIFFICULTY = 0;
  static constexpr int MAX_DIFFICULTY = 9;

  sf::Text difficultyText;
  sf::Text leftArrowText;
  sf::Text rightArrowText;
  sf::Text confirmButton;
  sf::Text backButton;

  static const int WINDOW_WIDTH = 500;
  static const int WINDOW_HEIGHT = 700;
  static const int TITLE_SIZE = 60;
  static const int ITEM_SIZE = 30;
  static const int ITEM_SPACING = 50;

  void updateDifficultyDisplay();

  // TODO: Dodać czcionki, teksty i obiekty graficzne SFML
  // TODO: Dodać tło menu

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
   * TODO: Załadować czcionki, utworzyć elementy menu
   */
  void initialize();

  /**
   * @brief Ustawienie stanu menu
   * @param state Nowy stan menu
   * TODO: Zmienić elementy menu w zależności od stanu
   */
  void setState(MenuState state);

  /**
   * @brief Pobierz aktualny stan
   */
  MenuState getState() const { return currentState; }

  /**
   * @brief Przesunięcie wyboru w górę
   * TODO: Zmienić selectedIndex z uwzględnieniem zawijania
   */
  void moveUp();

  /**
   * @brief Przesunięcie wyboru w dół
   * TODO: Zmienić selectedIndex z uwzględnieniem zawijania
   */
  void moveDown();

  /**
   * @brief Pobierz wybrany indeks opcji
   */
  int getSelectedIndex() const { return selectedIndex; }

  /**
   * @brief Obsługa wyboru opcji menu
   * @return Akcja do wykonania (jako enum lub int)
   * TODO: Zwrócić akcję odpowiadającą wybranej opcji (start gry, wyjście, itd.)
   */
  MenuAction handleSelection();

  /**
   * @brief Obsługa zdarzeń menu
   * @param event Zdarzenie SFML
   * TODO: Obsłużyć klawisze strzałek i Enter
   */
  void handleEvent(const sf::Event& event);

  // Zwiększa wybrany poziom trudności (z zawijaniem 5->1)
  void increaseDifficulty();
  
  // Zmniejsza wybrany poziom trudności (z zawijaniem 1->5)
  void decreaseDifficulty();
  
  // Zwraca wybrany poziom trudności (1-5)
  int getSelectedDifficulty() const { return selectedDifficulty; }
  
  // Konwertuje poziom trudności na prędkość spadania
  float getDifficultySpeed() const;

  // Sprawdza czy kliknięto w przyciski na ekranie wyboru trudności
  // Zwraca: 0 = nic, 1 = Confirm, 2 = Back
  int checkDifficultyClick(float mouseX, float mouseY) const;

  /**
   * @brief Renderowanie menu
   * @param window Okno do rysowania
   * TODO: Narysować tło, tytuł i opcje menu z podświetleniem wybranej opcji
   */
  void render(sf::RenderWindow& window) const;

  /**
   * @brief Wyświetlenie ekranu Game Over z wynikiem
   * @param score Końcowy wynik
   * @param isHighScore Czy to nowy rekord
   * TODO: Wyświetlić informacje o końcu gry i statystyki
   */
  void showGameOver(int score, bool isHighScore);
};

#endif  // MENU_HPP
