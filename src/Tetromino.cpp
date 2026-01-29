#include "Tetromino.h"
#include <algorithm>
#include <iostream>

extern ColorTheme CLASSIC_THEME; //  jak w GameEngine

Tetromino::Tetromino (TetrominoType tetrominoType)
    : x (3), y (0), type (tetrominoType) {

    std::cout << "[Tetromino] Tworzenie klocka typu: " << static_cast<int>(type) << '\n';

    //  domyślny motyw (żeby nie było śmieciowych kolorów)
    currentTheme = CLASSIC_THEME;

    //  UWAGA: tu ustawiamy TYLKO shape — kolor wyjdzie z motywu
    switch (type) {
    case TetrominoType::I:
        shape = { {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0} };
        break;

    case TetrominoType::O:
        shape = { {0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0} };
        break;

    case TetrominoType::T:
        shape = { {0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 0} };
        break;

    case TetrominoType::S:
        shape = { {0, 0, 0, 0}, {0, 0, 1, 1}, {0, 1, 1, 0}, {0, 0, 0, 0} };
        break;

    case TetrominoType::Z:
        shape = { {0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0} };
        break;

    case TetrominoType::J:
        shape = { {0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 1}, {0, 0, 0, 0} };
        break;

    case TetrominoType::L:
        shape = { {0, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 0, 0}, {0, 0, 0, 0} };
        break;

    default:
        shape = { {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0} };
        break;
    }

    updateColorFromTheme ();
}

void Tetromino::setTheme (const ColorTheme& theme) {
    currentTheme = theme;
    updateColorFromTheme ();
}

void Tetromino::updateColorFromTheme () {
    switch (type) {
    case TetrominoType::I: color = currentTheme.I; break;
    case TetrominoType::O: color = currentTheme.O; break;
    case TetrominoType::T: color = currentTheme.T; break;
    case TetrominoType::S: color = currentTheme.S; break;
    case TetrominoType::Z: color = currentTheme.Z; break;
    case TetrominoType::J: color = currentTheme.J; break;
    case TetrominoType::L: color = currentTheme.L; break;
    default: color = currentTheme.text; break;
    }
}

void Tetromino::render (sf::RenderWindow& window) {
    sf::RectangleShape block (sf::Vector2f (CELL_SIZE - 1, CELL_SIZE - 1));
    block.setFillColor (color);

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            if (shape[row][col] == 1) {
                block.setPosition (offsetX + (x + col) * CELL_SIZE,
                    offsetY + (y + row) * CELL_SIZE);
                window.draw (block);
            }
        }
    }
}

void Tetromino::renderGhost (sf::RenderWindow& window) {
    sf::RectangleShape block (sf::Vector2f (CELL_SIZE - 1, CELL_SIZE - 1));

    sf::Color ghostColor = color;
    ghostColor.a = 60;

    block.setFillColor (ghostColor);
    block.setOutlineThickness (0);

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            if (shape[row][col] == 1) {
                block.setPosition (offsetX + (x + col) * CELL_SIZE,
                    offsetY + (y + row) * CELL_SIZE);
                window.draw (block);
            }
        }
    }
}

void Tetromino::moveDown () { y++; }
void Tetromino::moveUp () { y--; }

void Tetromino::moveLeft () {
    x--;
    std::cout << "[Tetromino] Ruch w lewo: x = " << x << '\n';
}

void Tetromino::moveRight () {
    x++;
    std::cout << "[Tetromino] Ruch w prawo: x = " << x << '\n';
}

void Tetromino::rotate () {
    std::cout << "[Tetromino] Rotacja klocka\n";

    std::vector<std::vector<int>> rotatedShape (4, std::vector<int> (4, 0));

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            rotatedShape[col][row] = shape[row][col];
        }
    }

    for (int row = 0; row < 4; row++) {
        std::reverse (rotatedShape[row].begin (), rotatedShape[row].end ());
    }

    shape = rotatedShape;

    //  kolor się nie zmienia na rotacji, ale zostawiam to jako “bezpieczne”
    updateColorFromTheme ();
}
