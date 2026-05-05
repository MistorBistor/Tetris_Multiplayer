#pragma once
#include "ColorTheme.h"
#include "TetrominoType.h"
#include <SFML/Graphics.hpp>
#include <vector>

class Board {
private:
  static const int CELL_SIZE = 30;

  int offsetX;
  int offsetY;

  // Siatka: 0 = puste, 1 = zajęte
  std::vector<std::vector<TetrominoType>> grid;

  bool isClearing = false;
  float clearTimer = 0.0f;
  int blinkCount = 0;
  std::vector<int> clearingRows;

  float blinkInterval = 0.10f;         // co ile sekund zmiana (0.08–0.12 wygląda dobrze)
  int maxBlinks = 6; // 6 zmian = ok. 3 „mrugnięcia”
  bool blinkOn = false;

  ColorTheme currentTheme;

  sf::Color getColorForType(TetrominoType type);

public:
    static const int ROWS = 24;
    static const int COLS = 10;
  static const int HIDDEN_ROWS = 4;
  Board(); // konstruktor
  void render(sf::RenderWindow &window);
  bool isValidPosition(int x, int y) const;

  void lockTetromino(int x, int y, const std::vector<std::vector<int>> &shape, TetrominoType type);
  bool canPlaceTetromino(int x, int y, const std::vector<std::vector<int>> &shape) const;
  void lockTetromino(int x, int y, const std::vector<std::vector<int>> &shape);
  bool isLineFull(int row) const;
  void removeLine(int row);
  int clearFullLines();

  bool startClearAnimation(); // wykrywa pełne linie i uruchamia animację
  int updateClearAnimation(
      float dt); // zwraca >0 dopiero gdy zakończy i usunie linie
  bool isLineClearAnimating() const; // getter
  void updateClearAnimSpeed(int level);

  void reset();
  void setTheme(const ColorTheme &theme);

  // Do obsługi multiplayer:

  // Dodaj linie ataku od przeciwnika (szare klocki z dziurą)
  void addPenaltyLines(int lineCount);

  // Pobierz stan planszy jako wektor intów (do wysłania przez sieć)
  std::vector<std::vector<int>> getBoardState() const;

  // Ustaw stan planszy z otrzymanych danych (od przeciwnika)
  void setBoardState(const std::vector<std::vector<int>>& state);
  
  // Pobierz typ klocka w danej komórce (do renderowania planszy przeciwnika)
  TetrominoType getCellType(int row, int col) const;
};
