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

/**
 * Sprawdza czy dana linia jest cała wypełniona.
 * Linia jest pełna gdy wszystkie komórki (0 do COLS - 1) są zajęte (!= 0).
 */
bool Board::isLineFull(int row) const {
  for (int col = 0; col < COLS; col++) {
    if (grid[row][col] == 0) {
      return false;
    }
  }
  
  return true;
}

/**
 * Usuwa daną linię i opuszcza wszystkie bloki powyżej niej o 1 w dół.
 */
void Board::removeLine(int row) {
  for (int currentRow = row; currentRow > 0; currentRow--) {
    for (int col = 0; col < COLS; col++) {
      grid[currentRow][col] = grid[currentRow - 1][col];
    }
  }
  
  // Górna linia (row 0) staje się pusta
  for (int col = 0; col < COLS; col++) {
    grid[0][col] = 0;
  }
}

/**
 * Wykrywa wszystkie pełne linie, usuwa je i zwraca ich liczbę.
 * Sprawdzamy od dołu do góry, bo po usunięciu linii bloki opadają.
 */
int Board::clearFullLines() {
  int clearedLines = 0;
  
  // Sprawdzamy od dołu do góry (ROWS-1 do 0)
  int row = ROWS - 1;
  while (row >= 0) {
    if (isLineFull(row)) {
      removeLine(row);
      clearedLines++;
      // Nie zmniejszamy row, bo po usunięciu linia powyżej "spadła" na to miejsce
      // i musimy ją też sprawdzić
    } else {
      // Linia nie jest pełna, przechodzimy do następnej (wyżej)
      row--;
    }
  }
  
  return clearedLines;
}

/**
 * Resetuje planszę do stanu początkowego - wszystkie komórki puste.
 */
void Board::reset() {
  
  // Zerujemy całą siatkę
  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      grid[row][col] = TetrominoType::Empty;
    }
  }
  std::cout << "[Board] Reset planszy - wszystkie komórki ustawione na Empty\n";
}