#ifndef SCORE_HPP
#define SCORE_HPP

#include <SFML/Graphics.hpp>
#include <string>

/**
 * @class Score
 * @brief Zarządzanie punktacją w grze Tetris
 *
 * Klasa odpowiedzialna za śledzenie i obliczanie punktów,
 * poziomu trudności i statystyk gracza.
 */
class Score {
 private:
  int currentScore;
  int level;
  int linesCleared;
  int highScore;

  // TODO: Dodać zmienne do czcionek i tekstów do wyświetlania
  // TODO: Dodać tablicę punktów dla różnych akcji (1 linia, 2 linie, 3 linie,
  // Tetris)

 public:
  /**
   * @brief Konstruktor
   */
  Score();

  /**
   * @brief Destruktor
   */
  ~Score();

  /**
   * @brief Inicjalizacja systemu punktacji
   * TODO: Załadować czcionki, wczytać najwyższy wynik z pliku
   */
  void initialize();

  /**
   * @brief Dodanie punktów za usunięte linie
   * @param lines Liczba usuniętych linii jednocześnie
   * TODO: Obliczyć punkty według zasad Tetris (1x40, 2x100, 3x300, 4x1200)
   */
  void addScore(int lines);

  /**
   * @brief Zwiększenie poziomu
   * TODO: Zwiększać poziom co 10 linii
   */
  void increaseLevel();

  /**
   * @brief Obliczenie prędkości spadania na podstawie poziomu
   * TODO: Zwrócić czas w ms dla danego poziomu
   */
  float getFallSpeed() const;

  /**
   * @brief Reset punktacji
   * TODO: Zresetować wynik do stanu początkowego
   */
  void reset();

  /**
   * @brief Sprawdzenie czy osiągnięto nowy rekord
   * TODO: Porównać aktualny wynik z najwyższym
   */
  bool isNewHighScore() const;

  /**
   * @brief Zapis najwyższego wyniku do pliku
   * TODO: Zapisać highScore do pliku tekstowego lub binarnego
   */
  void saveHighScore() const;

  /**
   * @brief Wczytanie najwyższego wyniku z pliku
   * TODO: Wczytać highScore z pliku
   */
  void loadHighScore();

  /**
   * @brief Pobierz aktualny wynik
   */
  int getCurrentScore() const { return currentScore; }

  /**
   * @brief Pobierz poziom
   */
  int getLevel() const { return level; }

  /**
   * @brief Pobierz liczbę usuniętych linii
   */
  int getLinesCleared() const { return linesCleared; }

  /**
   * @brief Pobierz najwyższy wynik
   */
  int getHighScore() const { return highScore; }

  /**
   * @brief Renderowanie wyniku
   * @param window Okno do rysowania
   * TODO: Wyświetlić aktualny wynik, poziom, linie i najwyższy wynik
   */
  void render(sf::RenderWindow& window) const;
};

#endif  // SCORE_HPP
