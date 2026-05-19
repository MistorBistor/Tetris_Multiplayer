#include "Board.h"
#include "Tetromino.h"
#include <algorithm>
#include <iostream>
#include <random>

Board::Board()
    : isClearing(false), clearTimer(0.0f), blinkOn(true),
      grid(ROWS, std::vector<TetrominoType>(COLS, TetrominoType::Empty)) {
  reset();
}

/**
 * Rysowanie planszy.
 * Iterujemy przez siatkę i rysujemy każdy zajęty (niezerowy) kwadrat.
 */
void Board::render(sf::RenderWindow &window) {
  sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));

  // Offset dla planszy w oknie
  const int offsetX = 270;
  const int offsetY = 170;

  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      cell.setPosition(offsetX + col * CELL_SIZE,
                       offsetY + (row - HIDDEN_ROWS) * CELL_SIZE);

      // Jeśli trwa animacja usuwania linii
      if (isClearing && std::find(clearingRows.begin(), clearingRows.end(),
                                  row) != clearingRows.end()) {
        if (blinkOn) {
          // Błysk - biały
          cell.setFillColor(sf::Color::White);
          cell.setOutlineThickness(0);
        } else {
          // "zgaśnięcie" – wygląda jak pusta (używamy koloru tła z motywu)
          cell.setFillColor(currentTheme.background);
          cell.setOutlineThickness(1);
          cell.setOutlineColor(currentTheme.panel);
        }

        window.draw(cell);
        continue; // ❗ NIE rysujemy normalnej logiki
      }

      // NORMALNE RYSOWANIE
      if (grid[row][col] == TetrominoType::Empty) {
        // Jeśli pusty, rysujemy tylko jeśli NIE jest w ukrytym obszarze
        if (row >= HIDDEN_ROWS) {
          cell.setFillColor(currentTheme.background);
          cell.setOutlineThickness(1);
          cell.setOutlineColor(currentTheme.panel);
          window.draw(cell);
        }
      } else {
        // Jeśli zajęty, rysujemy ZAWSZE (nawet w ukrytym obszarze)
        cell.setFillColor(getColorForType(grid[row][col]));
        cell.setOutlineThickness(0);
        window.draw(cell);
      }
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
    return true; // pozwalamy na pozycje powyżej planszy
  }

  return grid[y][x] == TetrominoType::Empty;
}

/**
 * Blokowanie klocka na planszy oznacza ustawienie odpowiednich komórek w
 * siatce na typ klocka.
 */
void Board::lockTetromino(int x, int y, const std::vector<std::vector<int>> &shape, TetrominoType type) {
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      if (shape[row][col] == 1) {
        int boardX = x + col;
        int boardY = y + row;

        if (boardY >= 0 && boardY < ROWS && boardX >= 0 && boardX < COLS) {
          grid[boardY][boardX] = type;

          std::cout << "[Board] Blokuję komórkę (" << boardX << ", " << boardY
                    << ") typu " << static_cast<int>(type) << '\n';
        }
      }
    }
  }
}

/**
 * Sprawdza czy cały klocek może być umieszczony w danej pozycji.
 */
bool Board::canPlaceTetromino(int x, int y, const std::vector<std::vector<int>> &shape) const {
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      if (shape[row][col] == 1) { // Tylko wypełnione komórki
        int boardX = x + col;
        int boardY = y + row;

        // Sprawdź czy nie wychodzi poza planszę (lewo, prawo, dół)
        if (boardX < 0 || boardX >= COLS || boardY >= ROWS) {
          return false;
        }

        // Pozwalamy na pozycje powyżej planszy (podczas spawnu)
        if (boardY >= 0 && grid[boardY][boardX] != TetrominoType::Empty) {
          return false;
        }
      }
    }
  }
  return true; // Wszystkie komórki są OK
}

/**
 * Sprawdza czy dana linia jest cała wypełniona.
 */
bool Board::isLineFull(int row) const {
  for (int col = 0; col < COLS; col++) {
    if (grid[row][col] == TetrominoType::Empty) {
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
    grid[0][col] = TetrominoType::Empty;
  }
}

/**
 * Wykrywa wszystkie pełne linie, usuwa je i zwraca ich liczbę.
 */
int Board::clearFullLines() {
  int clearedLines = 0;

  // Sprawdzamy od dołu do góry
  int row = ROWS - 1;
  while (row >= 0) {
    if (isLineFull(row)) {
      removeLine(row);
      clearedLines++;
    } else {
      row--;
    }
  }

  return clearedLines;
}

bool Board::startClearAnimation() {
  if (isClearing)
    return true;

  clearingRows.clear();

  // Szukamy pełnych linii OD DOŁU
  for (int y = ROWS - 1; y >= 0; y--) {
    bool full = true;

    for (int x = 0; x < COLS; x++) {
      if (grid[y][x] == TetrominoType::Empty) {
        full = false;
        break;
      }
    }

    if (full) {
      clearingRows.push_back(y);
    }
  }

  if (clearingRows.empty()) {
    return false;
  }

  isClearing = true;
  clearTimer = 0.0f;
  blinkCount = 0;
  blinkOn = true;

  std::cout << "[Board] Start clear animation, rows:";
  for (int r : clearingRows)
    std::cout << " " << r;
  std::cout << "\n";

  return true;
}

int Board::updateClearAnimation(float dt) {
  if (!isClearing)
    return 0;

  // Mruganie
  clearTimer += dt;
  if (clearTimer >= blinkInterval) {
    clearTimer = 0.0f;
    blinkOn = !blinkOn;
    blinkCount++;
  }

  // Koniec animacji -> usuwamy linie
  if (blinkCount >= maxBlinks) {
    std::sort(clearingRows.begin(), clearingRows.end());

    int removed = (int)clearingRows.size();

    for (int row : clearingRows) {
      removeLine(row);
    }

    isClearing = false;
    clearingRows.clear();
    blinkOn = false;

    return removed;
  }

  return 0;
}

bool Board::isLineClearAnimating() const { return isClearing; }

/**
 * Resetuje planszę do stanu początkowego.
 */
void Board::reset() {
  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      grid[row][col] = TetrominoType::Empty;
    }
  }
  std::cout << "[Board] Reset planszy\n";
}

void Board::updateClearAnimSpeed(int level) {
  const float baseInterval = 0.10f;
  const float intervalPerLevel = 0.004f;
  const float minInterval = 0.04f;

  float interval = baseInterval - level * intervalPerLevel;
  if (interval < minInterval)
    interval = minInterval;

  blinkInterval = interval;
}

void Board::setTheme(const ColorTheme &theme) { currentTheme = theme; }

sf::Color Board::getColorForType(TetrominoType type) {
  switch (type) {
  case TetrominoType::I:
    return currentTheme.I;
  case TetrominoType::O:
    return currentTheme.O;
  case TetrominoType::T:
    return currentTheme.T;
  case TetrominoType::S:
    return currentTheme.S;
  case TetrominoType::Z:
    return currentTheme.Z;
  case TetrominoType::J:
    return currentTheme.J;
  case TetrominoType::L:
    return currentTheme.L;
  case TetrominoType::P:
    return currentTheme.P;
  default:
      return sf::Color::Transparent;
  }
}

void Board::addPenaltyLines(int lineCount) {
    if (lineCount <= 0) return;

    std::cout << "[Board] Dodawanie " << lineCount << " linii kary\n";

    // Generator liczb losowych dla pozycji dziury
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> holeDist(0, COLS - 1);

    // Przesuń całą planszę w górę o lineCount
    for (int row = 0; row < ROWS - lineCount; row++) {
        for (int col = 0; col < COLS; col++) {
            grid[row][col] = grid[row + lineCount][col];
        }
    }

    // Dodaj nowe linie na dole (szare klocki z dziurą)
    for (int addedRow = 0; addedRow < lineCount; addedRow++) {
        int rowIndex = ROWS - lineCount + addedRow;
        int holePosition = holeDist(rng);  // Losowa pozycja dziury

        for (int col = 0; col < COLS; col++) {
            if (col == holePosition) {
                grid[rowIndex][col] = TetrominoType::Empty;  // Dziura
            }
            else {
                // Szary klocek
                grid[rowIndex][col] = TetrominoType::P;
            }
        }
    }
}

std::vector<std::vector<int>> Board::getBoardState() const {
    std::vector<std::vector<int>> state(ROWS, std::vector<int>(COLS));

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            state[row][col] = static_cast<int>(grid[row][col]);
        }
    }

    return state;
}

void Board::setBoardState(const std::vector<std::vector<int>>& state) {
    if (state.size() != ROWS || state[0].size() != COLS) {
        std::cout << "[Board ERROR] Nieprawidłowy rozmiar stanu planszy!\n";
        return;
    }

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            grid[row][col] = static_cast<TetrominoType>(state[row][col]);
        }
    }
}

TetrominoType Board::getCellType(int row, int col) const {
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS) {
        return TetrominoType::Empty;
    }
    return grid[row][col];
}