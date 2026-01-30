#include "Tetromino.h"
#include "Board.h"
#include <algorithm>
#include <iostream>

extern ColorTheme CLASSIC_THEME;

Tetromino::Tetromino(TetrominoType tetrominoType)
    : x(3), y(1), type(tetrominoType) {

  // Fix: I piece spawns too low at y=1 because of its shape definition
  if (type == TetrominoType::I) {
    y = 0;
  }

  std::cout << "[Tetromino] Tworzenie klocka typu: " << static_cast<int>(type)
            << '\n';

  // domyślny motyw
  currentTheme = CLASSIC_THEME;

  switch (type) {
  case TetrominoType::I:
    shape = {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}};
    break;
  case TetrominoType::O:
    shape = {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}};
    break;
  case TetrominoType::T:
    shape = {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 0}};
    break;
  case TetrominoType::S:
    shape = {{0, 0, 0, 0}, {0, 0, 1, 1}, {0, 1, 1, 0}, {0, 0, 0, 0}};
    break;
  case TetrominoType::Z:
    shape = {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}};
    break;
  case TetrominoType::J:
    shape = {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 1}, {0, 0, 0, 0}};
    break;
  case TetrominoType::L:
    shape = {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 0, 0}, {0, 0, 0, 0}};
    break;
  default:
    shape = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
    break;
  }

  updateColorFromTheme();
}

void Tetromino::setTheme(const ColorTheme &theme) {
  currentTheme = theme;
  updateColorFromTheme();
}

void Tetromino::updateColorFromTheme() {
  switch (type) {
  case TetrominoType::I:
    color = currentTheme.I;
    break;
  case TetrominoType::O:
    color = currentTheme.O;
    break;
  case TetrominoType::T:
    color = currentTheme.T;
    break;
  case TetrominoType::S:
    color = currentTheme.S;
    break;
  case TetrominoType::Z:
    color = currentTheme.Z;
    break;
  case TetrominoType::J:
    color = currentTheme.J;
    break;
  case TetrominoType::L:
    color = currentTheme.L;
    break;
  default:
    color = currentTheme.text;
    break;
  }
}

void Tetromino::render(sf::RenderWindow &window) {
  sf::RectangleShape block(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
  block.setFillColor(color);

  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      if (shape[row][col] == 1) {
        int boardY = y + row;
        // Position is offset by HIDDEN_ROWS so it appears above the board
        block.setPosition(offsetX + (x + col) * CELL_SIZE,
                          offsetY + (boardY - Board::HIDDEN_ROWS) * CELL_SIZE);
        window.draw(block);
      }
    }
  }
}

void Tetromino::renderGhost(sf::RenderWindow &window) {
  sf::RectangleShape block(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));

  sf::Color ghostColor = color;
  ghostColor.a = 60;
  block.setFillColor(ghostColor);
  block.setOutlineThickness(0);

  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      if (shape[row][col] == 1) {
        int boardY = y + row;
        block.setPosition(offsetX + (x + col) * CELL_SIZE,
                          offsetY + (boardY - Board::HIDDEN_ROWS) * CELL_SIZE);
        window.draw(block);
      }
    }
  }
}

void Tetromino::moveDown() { y++; }
void Tetromino::moveUp() { y--; }

void Tetromino::moveLeft() { x--; }

void Tetromino::moveRight() { x++; }

void Tetromino::rotate() {
  std::vector<std::vector<int>> rotatedShape(4, std::vector<int>(4, 0));

  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      rotatedShape[col][row] = shape[row][col];
    }
  }

  for (int row = 0; row < 4; row++) {
    std::reverse(rotatedShape[row].begin(), rotatedShape[row].end());
  }

  shape = rotatedShape;
  updateColorFromTheme();
}
