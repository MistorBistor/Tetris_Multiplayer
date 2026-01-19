#include "Board.h"

// Offset oznacza pozycję planszy (Board) w oknie, czyli margines boczny
Board::Board() : offsetX(100), offsetY(50) {
  // Inicjalizacja pustej siatki, zgodnie z definicją w Board.h
  grid.resize(ROWS, std::vector<int>(COLS, 0));
}

void Board::render(sf::RenderWindow& window) {
  // Rysujemy siatkę
  sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));

  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      cell.setPosition(offsetX + col * CELL_SIZE, offsetY + row * CELL_SIZE);

      if (grid[row][col] == 0) {
        // Pusta komórka
        cell.setFillColor(sf::Color::White);
        cell.setOutlineThickness(1);
        cell.setOutlineColor(sf::Color::Black);
      } else {
        // Zablokowany klocek
        cell.setFillColor(sf::Color::Cyan);
        cell.setOutlineThickness(0);
      }

      window.draw(cell);
    }
  }
}

/**
 * Sprawdza czy pozycja jest prawidłowa (wewnątrz planszy i nie zajęta).
 */
bool Board::isValidPosition(int x, int y) const {
  if (x < 0 || x >= COLS || y >= ROWS) {
    return false;
  }

  if (y < 0) {
    return true;  // pozwalamy na pozycje powyżej planszy
  }

  return grid[y][x] == 0;
}

/**
 * Blokowanie klocka na planszy oznacza ustawienie odpowiednich komórek w
 * siatce na 1.
 */
void Board::lockTetromino(int x, int y,
                          const std::vector<std::vector<int>>& shape) {
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      if (shape[row][col] == 1) {
        int boardX = x + col;
        int boardY = y + row;

        if (boardY >= 0 && boardY < ROWS && boardX >= 0 && boardX < COLS) {
          grid[boardY][boardX] = 1;
        }
      }
    }
  }
}
