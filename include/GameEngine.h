#pragma once
#include <SFML/Graphics.hpp>
#include "Board.h"
#include "Menu.h"
#include "Score.h"
#include "Tetromino.h"
#include "TetrominoType.h"
#include <SFML/Audio.hpp>

class GameEngine {
private:
	sf::RenderWindow window;
	sf::Clock clock;

	Menu mainMenu; // Menu g³ówne gry

	enum class GameState { Menu, Playing, Paused, GameOver };
	GameState gameState = GameState::Menu;

	bool isRunning = false;

	// Spadanie klocka
	float fallTimer = 0.0f;
	float fallSpeed = 0.5f; // Im mniejsza wartoœæ, tym szybciej spada klocek

	// Lock delay: klocek "czeka" chwilê po dotkniêciu dna zanim siê zablokuje
	float lockTimer = 0.0f;
	float lockDelay = 0.5f;
	bool isLocking = false;

	// Anti infinite spin / lock delay move limit
	int lockMoveCounter = 0;
	const int maxLockMoves = 10;

	// czy ten klocek ju¿ kiedykolwiek dotkn¹³ pod³o¿a (dna/innego klocka)
	bool hasTouchedGround = false;

	// helper
	bool isPieceGrounded ();

	Board board;
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

	sf::Music backgroundMusic;

	// --- SFX (krótkie dŸwiêki) ---
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

	float sfxVolume = 50.f; // 0–100

	// Uwaga: brak konstruktora domyœlnego Tetromino, wiêc inicjalizujemy typem
	Tetromino currentTetromino = Tetromino (TetrominoType::I);

	int lastTetrominoType = -1;

	int startLevel = 0;        // level wybrany w menu (0–9)
	int currentLevel = 0;      // aktualny level w grze
	int totalLinesCleared = 0; // ile linii wyczyszczono od startu

	float getFallSpeedForLevel (int level) const;
	void updateLevelAndSpeed (int linesJustCleared);

	void updateMusicSpeed ();

	void renderGhostTetromino ();

	

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
  void loadAudio ();

  // Menu / UI
  
  void handleMenuSelection();
  void renderGameOver();

  //Metody pomocnicze next/hold
  TetrominoType getRandomTetrominoType();
  void holdCurrentPiece();
  void renderNextPiece(sf::RenderWindow& window) const;
  void renderHeldPiece(sf::RenderWindow& window) const;
  void renderTetrominoPreview(sf::RenderWindow& window, TetrominoType type, float x, float y) const;
  
  //ObsÅ‚uga kontrolera Xbox Series X
  void handleControllerButton(unsigned int button);
  void handleControllerAxis(sf::Joystick::Axis axis, float position);
  void handleControllerButtonMenu(unsigned int button);
  void handleControllerAxisMenu(sf::Joystick::Axis axis, float position);

};