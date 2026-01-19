#include "Tetromino.h"

/**
 * Inicjalizuje początkowe Tetromino o kształcie I.
 */
Tetromino::Tetromino() : x(3), y(0), color(sf::Color::Cyan) {
  shape = {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}};
}

void Tetromino::render(sf::RenderWindow& window) {
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
