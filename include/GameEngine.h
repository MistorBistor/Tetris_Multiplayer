#pragma once
#include <SFML/Graphics.hpp>

#include "Board.h"
#include "Menu.h"
#include "Tetromino.h"

class GameEngine {
 private:
  sf::RenderWindow window;
  sf::Clock clock;
  Menu menu;
  bool isRunning = false;
  float fallTimer = 0.0f;
  /**
   * Zakres wartości od 0.1 do 1.0 określa jak szybko klocek spada.
   */
  float fallSpeed = 0.5f;

  enum class GameState { Menu, Playing, Paused, GameOver };
  GameState gameState = GameState::Menu;

  Board board;
  Tetromino currentTetromino;

 public:
  GameEngine();
  void initialize();
  void run();
  void shutdown();

 private:
  void handleEvents();
  void update(float deltaTime);
  void render();

  // Metody pomocnicze
  bool checkCollision();
  void lockTetromino();
  void spawnNewTetromino();
  void handleMenuSelection();
  void renderGameOver();
};
