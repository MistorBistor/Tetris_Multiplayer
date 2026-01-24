#include "Tetromino.h"
#include <algorithm>
#include <iostream>

Tetromino::Tetromino(TetrominoType tetrominoType)
    : x(3), y(0), type(tetrominoType) { // ← To jest OTWARCIE {

  std::cout << "[Tetromino] Tworzenie klocka typu: " << static_cast<int>(type)
            << '\n';

  switch (type) {
  case TetrominoType::I: // Cyan - linia pionowa
    color = sf::Color::Cyan;
    shape = {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}};
    break;

  case TetrominoType::O: // Yellow - kwadrat
    color = sf::Color::Yellow;
    shape = {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}};
    break;

  case TetrominoType::T: // Magenta - T-shape
    color = sf::Color::Magenta;
    shape = {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 0}};
    break;

  case TetrominoType::S: // Green - S-shape
    color = sf::Color::Green;
    shape = {{0, 0, 0, 0}, {0, 0, 1, 1}, {0, 1, 1, 0}, {0, 0, 0, 0}};
    break;

  case TetrominoType::Z: // Red - Z-shape
    color = sf::Color::Red;
    shape = {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}};
    break;

  case TetrominoType::J: // Blue - J-shape
    color = sf::Color::Blue;
    shape = {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 1}, {0, 0, 0, 0}};
    break;

  case TetrominoType::L:            // Orange - L-shape
    color = sf::Color(255, 165, 0); // RGB dla pomarańczowego
    shape = {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 0, 0}, {0, 0, 0, 0}};
    break;
  }
} // ← A to jest ZAMKNIĘCIE }

void Tetromino::render(sf::RenderWindow &window) {
  sf::RectangleShape block(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));

  block.setFillColor(color);

  // Tutaj rysujemy tylko te części klocka, które są wypełnione (wartość 1)
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      if (shape[row][col] == 1) {
        block.setPosition(offsetX + (x + col) * CELL_SIZE,
                          offsetY + (y + row) * CELL_SIZE);
        window.draw(block);
      }
    }
  }
}

void Tetromino::moveDown() { y++; }

void Tetromino::moveUp() { y--; }

/**
 * Przesuwa klocek o jedną pozycję w lewo.
 * Zmniejsza współrzędną x o 1.
 */
void Tetromino::moveLeft() {
  x--;
  std::cout << "[Tetromino] Ruch w lewo: x = " << x << '\n';
}

/**
 * Przesuwa klocek o jedną pozycję w prawo.
 * Zwiększa współrzędną x o 1.
 */
void Tetromino::moveRight() {
  x++;
  std::cout << "[Tetromino] Ruch w prawo: x = " << x << '\n';
}

/**
 * Obraca klocek o 90 stopni w prawo (zgodnie z ruchem wskazówek zegara).
 * Algorytm:
 * 1. Transponowanie macierzy (zamiana wierszy z kolumnami)
 * 2. Odwrócenie każdego wiersza
 */
void Tetromino::rotate() {
  std::cout << "[Tetromino] Rotacja klocka\n";

  // Tworzymy nową macierz 4x4 dla wyniku rotacji
  std::vector<std::vector<int>> rotatedShape(4, std::vector<int>(4, 0));

  // Krok 1: Transponowanie - zamiana shape[row][col] na rotatedShape[col][row]
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      rotatedShape[col][row] = shape[row][col];
    }
  }

  // Krok 2: Odwrócenie każdego wiersza (lewo-prawo)
  for (int row = 0; row < 4; row++) {
    std::reverse(rotatedShape[row].begin(), rotatedShape[row].end());
  }

  // Zastąp stary kształt nowym
  shape = rotatedShape;
}
