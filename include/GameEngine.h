#pragma once
#include <SFML/Graphics.hpp>
#include "Board.h"
#include "Menu.h"
#include "Score.h"
#include "Tetromino.h"
#include "TetrominoType.h"

class GameEngine {
private:
  sf::RenderWindow window;
  sf::Clock clock;
  Menu mainMenu;
  Score score;
  
  enum class GameState { Menu, Playing, Paused, GameOver };
  GameState gameState = GameState::Menu;
  bool isRunning = false;
  
  float fallTimer = 0.0f;
  float fallSpeed = 0.5f;
  float lockTimer = 0.0f;
  float lockDelay = 0.5f;
  bool isLocking = false;
  
  Board board;
  Tetromino currentTetromino = Tetromino(TetrominoType::I);

  std::vector<TetrominoType> nextQueue;
  TetrominoType heldTetrominoType;
  bool hasHeldPiece;
  bool canHold;  // Czy mo¿na u¿yæ hold w tej turze (raz na klocek)
  
  sf::Font uiFont;
  
public:
  GameEngine();
  void initialize();
  void run();
  void shutdown();

  private:
  void handleEvents();
  void update(float deltaTime);
  void render();

  void handleKeyPress(sf::Keyboard::Key key);
  
  bool checkCollision();
  void lockTetromino();
  void spawnNewTetromino();
  
  void handleMenuSelection();
  void renderGameOver();

  //Metody pomocnicze next/hold
  TetrominoType getRandomTetrominoType();
  void holdCurrentPiece();
  void renderNextPiece(sf::RenderWindow& window) const;
  void renderHeldPiece(sf::RenderWindow& window) const;
  void renderTetrominoPreview(sf::RenderWindow& window, TetrominoType type, float x, float y) const;
  
  //Obs³uga kontrolera Xbox Series X
  void handleControllerButton(unsigned int button);
  void handleControllerAxis(sf::Joystick::Axis axis, float position);
  void handleControllerButtonMenu(unsigned int button);
  void handleControllerAxisMenu(sf::Joystick::Axis axis, float position);

};