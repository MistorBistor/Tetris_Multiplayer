#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Board {
 private:
  static const int ROWS = 20;
  static const int COLS = 10;
  static const int CELL_SIZE = 30;

  int offsetX;
  int offsetY;

  // Siatka: 0 = puste, 1 = zajęte
  std::vector<std::vector<int>> grid;

 public:
  Board();  // konstruktor
  void render(sf::RenderWindow& window);
  bool isValidPosition(int x, int y) const;
  void lockTetromino(int x, int y, const std::vector<std::vector<int>>& shape);
};
