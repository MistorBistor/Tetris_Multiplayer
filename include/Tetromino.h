#pragma once
#include "ColorTheme.h"
#include "TetrominoType.h"
#include <SFML/Graphics.hpp>
#include <vector>

class Tetromino {
private:
  int x, y; // pozycja klocka na planszy
  sf::Color color;
  std::vector<std::vector<int>> shape; // 4x4 mapa klocka
  TetrominoType type;

  // motyw
  ColorTheme currentTheme;

  static const int CELL_SIZE = 30;
  // Offset oznacza pozycję planszy (Board) w oknie, czyli margines
  int offsetX = 270;
  int offsetY = 170; // KEEP MAIN'S 170 for spawn above board

  void updateColorFromTheme(); // nowa funkcja pomocnicza

public:
  Tetromino(TetrominoType tetrominoType);

  // GameEngine/Board może ustawić motyw na klocek
  void setTheme(const ColorTheme &theme);

  void render(sf::RenderWindow &window);
  void renderGhost(sf::RenderWindow &window);

  void moveDown();
  void moveUp();
  void moveLeft();
  void moveRight();
  void rotate();

  int getX() const { return x; }
  int getY() const { return y; }
  const std::vector<std::vector<int>> &getShape() const { return shape; }
  TetrominoType getType() const { return type; }
};
