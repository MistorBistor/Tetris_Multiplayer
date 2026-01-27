#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Board.h"
#include "Menu.h"
#include "Score.h"
#include "Tetromino.h"
#include "TetrominoType.h"
#include <vector>

class GameEngine {
private:
  sf::RenderWindow window;
  sf::Clock clock;
  Menu mainMenu;
  Score score;
  
  enum class GameState { Menu, Playing, Paused, GameOver };
  GameState gameState = GameState::Menu;
  bool isRunning = false;
  
  // Spadanie klocka
  float fallTimer = 0.0f;
  float fallSpeed = 0.5f;
  float lockTimer = 0.0f;
  float lockDelay = 0.5f;
  bool isLocking = false;
  
  // Anti infinite spin / lock delay move limit
  int lockMoveCounter = 0;
  const int maxLockMoves = 10;
  bool hasTouchedGround = false;
  
  Board board;
  Tetromino currentTetromino = Tetromino(TetrominoType::I);
  
  // Audio
  sf::Music backgroundMusic;
  sf::SoundBuffer sfxMenuBuf;
  sf::SoundBuffer sfxMoveBuf;
  sf::SoundBuffer sfxRotateBuf;
  sf::SoundBuffer sfxLandedBuf;
  sf::SoundBuffer sfxLineClearBuf;
  sf::SoundBuffer sfxGameOverBuf;
  sf::Sound sfxMenu;
  sf::Sound sfxMove;
  sf::Sound sfxRotate;
  sf::Sound sfxLanded;
  sf::Sound sfxLineClear;
  sf::Sound sfxGameOver;
  float sfxVolume = 50.f;
  
  // Level system
  int lastTetrominoType = -1;
  int startLevel = 0;
  int currentLevel = 0;
  int totalLinesCleared = 0;
  
  // Next/Hold system
  std::vector<TetrominoType> nextQueue;
  TetrominoType heldTetrominoType;
  bool hasHeldPiece;
  bool canHold;
  
  sf::Font uiFont;
  
  // Helper methods
  bool isPieceGrounded();
  float getFallSpeedForLevel(int level) const;
  void updateLevelAndSpeed(int linesJustCleared);
  void updateMusicSpeed();
  void renderGhostTetromino();
  
  void handleEvents();
  void update(float deltaTime);
  void render();
  void handleKeyPress(sf::Keyboard::Key key);
  
  bool checkCollision();
  void lockTetromino();
  void spawnNewTetromino();
  void loadAudio();
  
  void handleMenuSelection();
  void renderGameOver();
  
  // Next/Hold helpers
  TetrominoType getRandomTetrominoType();
  void holdCurrentPiece();
  void renderNextPiece(sf::RenderWindow& window) const;
  void renderHeldPiece(sf::RenderWindow& window) const;
  void renderTetrominoPreview(sf::RenderWindow& window, TetrominoType type, float x, float y) const;
  
  // Controller support
  void handleControllerButton(unsigned int button);
  void handleControllerAxis(sf::Joystick::Axis axis, float position);
  void handleControllerButtonMenu(unsigned int button);
  void handleControllerAxisMenu(sf::Joystick::Axis axis, float position);

  // Game Over state
  std::string playerName;
  int selectedGameOverElement;  // 0 = input, 1 = Confirm, 2 = Main Menu
  bool isTypingName;

  void handleGameOverInput(sf::Event& event);
  void saveHighScore();

public:
  GameEngine();
  void initialize();
  void run();
  void shutdown();
};