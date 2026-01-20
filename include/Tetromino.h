#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "TetrominoType.h"

class Tetromino {
 private:
  int x, y;  // pozycja klocka na planszy
  sf::Color color;
  std::vector<std::vector<int>> shape;  // 4x4 mapa klocka

  TetrominoType type;

  static const int CELL_SIZE = 30;
  // Offset oznacza pozycję planszy (Board) w oknie, czyli margines
  int offsetX = 100;
  int offsetY = 50;

 public:
  Tetromino(TetrominoType tetrominoType);
  void render(sf::RenderWindow& window);
  void moveDown();
  int getX() const { return x; }
  int getY() const { return y; }
  const std::vector<std::vector<int>>& getShape() const { return shape; }
  TetrominoType getType () const { return type; }
  void moveUp();
  void moveLeft ();
  void moveRight ();
  void rotate ();
};
