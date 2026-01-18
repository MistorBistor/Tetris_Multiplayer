#ifndef MENU_HPP
#define MENU_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

/**
 * @enum MenuState
 * @brief Stany menu
 */
enum class MenuState {
    MAIN_MENU,
    GAME,
    PAUSE,
    GAME_OVER,
    SETTINGS
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
    int handleSelection();
    
    /**
     * @brief Obsługa zdarzeń menu
     * @param event Zdarzenie SFML
     * TODO: Obsłużyć klawisze strzałek i Enter
     */
    void handleEvent(const sf::Event& event);
    
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

#endif // MENU_HPP
