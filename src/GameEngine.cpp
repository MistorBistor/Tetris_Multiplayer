#include "GameEngine.h"

#include <iostream>

GameEngine::GameEngine() : window(sf::VideoMode(500, 700), "Tetris") {}

void GameEngine::initialize() {
  isRunning = true;
  gameState = GameState::Playing;

  std::cout << "Inicjalizacja gry" << '\n';
}

void GameEngine::run() {
  while (window.isOpen()) {
    // Oblicz deltaTime, czyli czas, który upłynął od ostatniej klatki
    float deltaTime = clock.restart().asSeconds();

    handleEvents();

    if (gameState == GameState::Playing) {
      update(deltaTime);
    }

    render();
  }
}

void GameEngine::handleEvents() {
  sf::Event event;

  while (window.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      window.close();
    }
  }
}

/**
 * Aktualizuje stan gry na podstawie upływu czasu.
 * Dodajemy do fallTimer czas, który upłynął od ostatniej klatki (deltaTime).
 * Gdy fallTimer przekroczy wartość fallSpeed (domyślnie 1 sekunda),
 * resetujemy fallTimer i przesuwamy aktualny klocek Tetromino o jedną
 * jednostkę w dół.
 */
void GameEngine::update(float deltaTime) {
  fallTimer += deltaTime;
  if (fallTimer >= fallSpeed) {
    fallTimer = 0.0f;

    std::cout << "Klocek spada: y" << " = " << currentTetromino.getY() << '\n';
    currentTetromino.moveDown();

    if (checkCollision()) {
      // Klocek spadł za nisko, cofamy ruch
      currentTetromino.moveUp();

      lockTetromino();

      spawnNewTetromino();
      // FIXME: Gra nie powinna się kończyć w tym miejscu
      //gameState = GameState::GameOver;

      //std::cout << "GAME OVER - klocek dotknął dna" << '\n';
    }
  }
}

void GameEngine::render() {
  window.clear(sf::Color::White);
  board.render(window);

  if (gameState == GameState::Playing) {
    currentTetromino.render(window);
  } else if (gameState == GameState::GameOver) {
    board.render(window);  // renderuj te klocki, które zostały "zablokowane"
    renderGameOver();
  }

  window.display();
}

void GameEngine::shutdown() { std::cout << "Koniec gry" << '\n'; }

bool GameEngine::checkCollision() {
  const auto& shape = currentTetromino.getShape();
  int tetrominoX = currentTetromino.getX();
  int tetrominoY = currentTetromino.getY();

  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      if (shape[row][col] == 1) {
        int boardX = tetrominoX + col;
        int boardY = tetrominoY + row;

        if (!board.isValidPosition(boardX, boardY)) {
          return true;
        }
      }
    }
  }

  return false;
}

void GameEngine::lockTetromino() {
  const auto& shape = currentTetromino.getShape();
  int tetrominoX = currentTetromino.getX();
  int tetrominoY = currentTetromino.getY();

  board.lockTetromino(tetrominoX, tetrominoY, shape);
  std::cout << "Klocek został zablokowany na planszy, pozycja: (" << tetrominoX
            << ", " << tetrominoY << ")\n";
  
  // Sprawdź i usuń pełne linie
  int linesCleared = board.clearFullLines();
  if (linesCleared > 0) {
    std::cout << "Wyczyszczono " << linesCleared << " linie!" << '\n';
  }
  //Tworzy nowy klocek Tetromino
  spawnNewTetromino();
}

/**
 * Tworzy nowy klocek Tetromino.
 * Na razie zawsze tworzymy klocek I w pozycji startowej (3, 0).
 */
void GameEngine::spawnNewTetromino() {
  currentTetromino = Tetromino();
  std::cout << "Nowy klocek utworzony na pozycji (3, 0)" << '\n';
}

void GameEngine::renderGameOver() {
  // Półprzezroczyste czarne tło na całym oknie
  sf::RectangleShape overlay(sf::Vector2f(500, 700));
  overlay.setPosition(0, 0);
  overlay.setFillColor(sf::Color(0, 0, 0, 150));
  window.draw(overlay);
}


