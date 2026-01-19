#ifndef TETROMINO_HPP
#define TETROMINO_HPP

#include <SFML/Graphics.hpp>
#include <vector>

/**
 * @enum TetrominoType
 * @brief Typy klocków Tetris
 */
enum class TetrominoType {
  I,  // Klocek liniowy
  O,  // Kwadrat
  T,  // Kształt T
  S,  // Kształt S
  Z,  // Kształt Z
  J,  // Kształt J
  L   // Kształt L
};

/**
 * @class Tetromino
 * @brief Klasa reprezentująca klocek Tetris (tetromino)
 *
 * Zarządza kształtem, kolorem, rotacją i pozycją klocka.
 */
class Tetromino {
 private:
  TetrominoType type;
  std::vector<std::vector<int>> shape;
  sf::Color color;
  int currentRotation;
  int x, y;

  // TODO: Dodać definicje kształtów dla każdego typu tetromino
  // TODO: Dodać tablice rotacji dla każdego kształtu

 public:
  /**
   * @brief Konstruktor z typem tetromino
   * @param type Typ klocka
   */
  Tetromino(TetrominoType type);

  /**
   * @brief Konstruktor losowy
   * TODO: Utworzyć losowy tetromino
   */
  Tetromino();

  /**
   * @brief Destruktor
   */
  ~Tetromino();

  /**
   * @brief Obrót klocka w prawo (90 stopni)
   * TODO: Implementować rotację macierzy kształtu
   */
  void rotateRight();

  /**
   * @brief Obrót klocka w lewo (90 stopni)
   * TODO: Implementować rotację macierzy kształtu
   */
  void rotateLeft();

  /**
   * @brief Przesunięcie klocka w lewo
   */
  void moveLeft();

  /**
   * @brief Przesunięcie klocka w prawo
   */
  void moveRight();

  /**
   * @brief Przesunięcie klocka w dół
   */
  void moveDown();

  /**
   * @brief Pobierz kształt tetromino
   * TODO: Zwrócić aktualny kształt z uwzględnieniem rotacji
   */
  const std::vector<std::vector<int>>& getShape() const;

  /**
   * @brief Pobierz kolor tetromino
   */
  sf::Color getColor() const { return color; }

  /**
   * @brief Pobierz typ tetromino
   */
  TetrominoType getType() const { return type; }

  /**
   * @brief Pobierz pozycję X
   */
  int getX() const { return x; }

  /**
   * @brief Pobierz pozycję Y
   */
  int getY() const { return y; }

  /**
   * @brief Ustaw pozycję
   */
  void setPosition(int newX, int newY);

  /**
   * @brief Renderowanie tetromino
   * @param window Okno do rysowania
   * TODO: Narysować tetromino na podstawie kształtu i pozycji
   */
  void render(sf::RenderWindow& window) const;
};

#endif  // TETROMINO_HPP
