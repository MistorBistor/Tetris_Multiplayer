#include "Board.h"
#include <algorithm>
#include <iostream>

/**
 * Zwraca kolor na podstawie typu klocka.
 * Używamy tych samych kolorów co w Tetromino.
 */
sf::Color getColorForType(TetrominoType type) {
  switch (type) {
  case TetrominoType::I:
    return sf::Color::Cyan;
  case TetrominoType::O:
    return sf::Color::Yellow;
  case TetrominoType::T:
    return sf::Color::Magenta;
  case TetrominoType::S:
    return sf::Color::Green;
  case TetrominoType::Z:
    return sf::Color::Red;
  case TetrominoType::J:
    return sf::Color::Blue;
  case TetrominoType::L:
    return sf::Color(255, 165, 0); // Orange
  default:
    return sf::Color::White; // Empty lub nieznany
  }
}

// Offset oznacza pozycję planszy (Board) w oknie, czyli margines boczny
Board::Board() : offsetX(270), offsetY(170) {
  grid.resize(ROWS, std::vector<TetrominoType>(COLS, TetrominoType::Empty));
}

void Board::render(sf::RenderWindow &window) {
  sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));

  for (int row = 0; row < ROWS; row++) {
    // 🔥 Czy ten wiersz jest aktualnie czyszczony?
    bool rowClearing =
        isClearing && (std::find(clearingRows.begin(), clearingRows.end(),
                                 row) != clearingRows.end());

    for (int col = 0; col < COLS; col++) {
      // Adjust Y position by subtracting HIDDEN_ROWS from row index
      cell.setPosition(offsetX + col * CELL_SIZE,
                       offsetY + (row - HIDDEN_ROWS) * CELL_SIZE);

      // ANIMACJA CZYSZCZENIA LINII
      if (rowClearing) {
        if (blinkOn) {
          // "rozbłysk" – jasny kolor
          cell.setFillColor(sf::Color::Black); // możesz zmienić na White
          cell.setOutlineThickness(0);
        } else {
          // "zgaśnięcie" – wygląda jak pusta
          cell.setFillColor(sf::Color::White);
          cell.setOutlineThickness(1);
          cell.setOutlineColor(sf::Color::Black);
        }

        window.draw(cell);
        continue; // ❗ NIE rysujemy normalnej logiki
      }

      // NORMALNE RYSOWANIE
      if (grid[row][col] == TetrominoType::Empty) {
        // Jeśli pusty, rysujemy tylko jeśli NIE jest w ukrytym obszarze
        if (row >= HIDDEN_ROWS) {
          cell.setFillColor(sf::Color::White);
          cell.setOutlineThickness(1);
          cell.setOutlineColor(sf::Color::Black);
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
 * siatce na 1.
 */
void Board::lockTetromino(int x, int y,
                          const std::vector<std::vector<int>> &shape,
                          TetrominoType type) {
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
 * Iteruje przez wszystkie wypełnione komórki klocka (wartość 1)
 * i sprawdza czy każda z nich jest w prawidłowej pozycji.
 */
bool Board::canPlaceTetromino(
    int x, int y, const std::vector<std::vector<int>> &shape) const {
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      if (shape[row][col] == 1) { // Tylko wypełnione komórki
        int boardX = x + col;
        int boardY = y + row;

        // Sprawdź czy nie wychodzi poza planszę (lewo, prawo, dół)
        if (boardX < 0 || boardX >= COLS || boardY >= ROWS) {
          std::cout << "[Board] Kolizja: poza planszą (" << boardX << ", "
                    << boardY << ")\n";
          return false;
        }

        // Pozwalamy na pozycje powyżej planszy (podczas spawnu)
        if (boardY >= 0 && grid[boardY][boardX] != TetrominoType::Empty) {
          std::cout << "[Board] Kolizja: zajęta komórka (" << boardX << ", "
                    << boardY << ")\n";
          return false;
        }
      }
    }
  }
  return true; // Wszystkie komórki są OK
}

/**
 * Sprawdza czy dana linia jest cała wypełniona.
 * Linia jest pełna gdy wszystkie komórki (0 do COLS - 1) są zajęte (!= 0).
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
      // Nie zmniejszamy row, bo po usunięciu linia powyżej "spadła" na to
      // miejsce i musimy ją też sprawdzić
    } else {
      // Linia nie jest pełna, przechodzimy do następnej (wyżej)
      row--;
    }
  }

  return clearedLines;
}

bool Board::startClearAnimation() {
  if (isClearing)
    return true;

  clearingRows.clear();

  // Szukamy pełnych linii OD DOŁU (ważne!)
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
    // Usuwamy od góry do dołu (rosnąco), żeby removeLine nie rozjechał indeksów
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

void Board::updateClearAnimSpeed(int level) {
  // bazowy interwał (na niskim levelu)
  const float baseInterval = 0.10f;

  // jak szybko przyspieszamy z levelem
  const float intervalPerLevel = 0.004f;

  // minimalny interwał (żeby nie było epilepsji i nie zniknęło "instant")
  const float minInterval = 0.04f;

  float interval = baseInterval - level * intervalPerLevel;
  if (interval < minInterval)
    interval = minInterval;

  blinkInterval = interval;

  // maxBlinks zostawiamy stałe (np. 6), żeby nadal było 2-3 mrugnięcia
  // jeśli chcesz, możesz też delikatnie skracać liczbę blinków:
  // maxBlinks = std::max(4, 6 - level / 5);
}
