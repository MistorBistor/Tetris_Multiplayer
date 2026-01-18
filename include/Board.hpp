#ifndef BOARD_HPP
#define BOARD_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include "Tetromino.hpp"

/**
 * @class Board
 * @brief Plansza gry Tetris
 * 
 * Klasa reprezentująca planszę gry, zarządzająca siatką bloków
 * i wykrywaniem kolizji.
 */
class Board {
private:
    static const int BOARD_WIDTH;
    static const int BOARD_HEIGHT;
    static const int CELL_SIZE;
    
    std::vector<std::vector<int>> grid;
    // TODO: Dodać zmienne do przechowywania kolorów komórek
    // TODO: Dodać zmienne do pozycji planszy na ekranie
    
public:
    /**
     * @brief Konstruktor planszy
     */
    Board();
    
    /**
     * @brief Destruktor
     */
    ~Board();
    
    /**
     * @brief Inicjalizacja planszy
     * TODO: Zainicjalizować siatkę pustymi komórkami
     */
    void initialize();
    
    /**
     * @brief Sprawdzenie czy tetromino może być umieszczone w danej pozycji
     * @param tetromino Tetromino do sprawdzenia
     * @param x Pozycja X
     * @param y Pozycja Y
     * TODO: Implementować wykrywanie kolizji z granicami i innymi blokami
     */
    bool canPlaceTetromino(const Tetromino& tetromino, int x, int y) const;
    
    /**
     * @brief Umieszczenie tetromino na planszy
     * @param tetromino Tetromino do umieszczenia
     * @param x Pozycja X
     * @param y Pozycja Y
     * TODO: Dodać tetromino do siatki planszy
     */
    void placeTetromino(const Tetromino& tetromino, int x, int y);
    
    /**
     * @brief Sprawdzenie i usunięcie zapełnionych linii
     * @return Liczba usuniętych linii
     * TODO: Implementować sprawdzanie zapełnionych linii i ich usuwanie
     */
    int clearFullLines();
    
    /**
     * @brief Sprawdzenie czy gra się skończyła
     * TODO: Sprawdzić czy nowe klocki mogą się pojawić
     */
    bool isGameOver() const;
    
    /**
     * @brief Wyczyszczenie planszy
     * TODO: Zresetować wszystkie komórki
     */
    void clear();
    
    /**
     * @brief Renderowanie planszy
     * @param window Okno do rysowania
     * TODO: Narysować siatkę i wszystkie umieszczone bloki
     */
    void render(sf::RenderWindow& window) const;
    
    /**
     * @brief Pobierz szerokość planszy
     */
    int getWidth() const { return BOARD_WIDTH; }
    
    /**
     * @brief Pobierz wysokość planszy
     */
    int getHeight() const { return BOARD_HEIGHT; }
};

#endif // BOARD_HPP
