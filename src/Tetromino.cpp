#include "Tetromino.hpp"

#include <cstdlib>
#include <ctime>

Tetromino::Tetromino(TetrominoType type)
    : type(type), currentRotation(0), x(0), y(0) {
  // TODO: Na podstawie typu, zainicjalizować kształt i kolor
  // TODO: I-piece: cyan, O-piece: yellow, T-piece: purple,
  //       S-piece: green, Z-piece: red, J-piece: blue, L-piece: orange

  // Tymczasowa inicjalizacja, aby zapobiec niezdefiniowanemu zachowaniu
  shape = {{0}};
  color = sf::Color::White;
}

Tetromino::Tetromino() : currentRotation(0), x(0), y(0) {
  // TODO: Zainicjalizować generator liczb losowych (jeśli nie został)
  // TODO: Wylosować typ tetromino (0-6)
  // TODO: Wywołać konstruktor z typem

  // Tymczasowa inicjalizacja
  type = TetrominoType::I;
  shape = {{0}};
  color = sf::Color::White;
}

Tetromino::~Tetromino() {
  // TODO: Cleanup jeśli potrzebne
}

void Tetromino::rotateRight() {
  // TODO: Obrócić macierz shape o 90 stopni w prawo
  // TODO: Algorytm: transponować macierz, następnie odwrócić każdy wiersz
}

void Tetromino::rotateLeft() {
  // TODO: Obrócić macierz shape o 90 stopni w lewo
  // TODO: Algorytm: odwrócić każdy wiersz, następnie transponować macierz
}

void Tetromino::moveLeft() { x--; }

void Tetromino::moveRight() { x++; }

void Tetromino::moveDown() { y++; }

const std::vector<std::vector<int>>& Tetromino::getShape() const {
  // TODO: Zwrócić shape z uwzględnieniem aktualnej rotacji
  return shape;
}

void Tetromino::setPosition(int newX, int newY) {
  x = newX;
  y = newY;
}

void Tetromino::render(sf::RenderWindow& window) const {
  // TODO: Dla każdego bloku w shape:
  //       - jeśli wartość != 0, narysować kwadrat na pozycji (x+col, y+row)
  // TODO: Użyć koloru tetromino i odpowiedniego rozmiaru komórki
}
