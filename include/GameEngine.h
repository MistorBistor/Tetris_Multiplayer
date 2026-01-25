#pragma once
#include <SFML/Graphics.hpp>

#include "Board.h"
#include "Menu.h"
#include "Tetromino.h"
#include "TetrominoType.h"
#include <SFML/Audio.hpp>

class GameEngine {
private:
	sf::RenderWindow window;
	sf::Clock clock;

	Menu mainMenu; // Menu główne gry

	enum class GameState { Menu, Playing, Paused, GameOver };
	GameState gameState = GameState::Menu;

	bool isRunning = false;

	// Spadanie klocka
	float fallTimer = 0.0f;
	float fallSpeed = 0.5f; // Im mniejsza wartość, tym szybciej spada klocek

	// Lock delay: klocek "czeka" chwilę po dotknięciu dna zanim się zablokuje
	float lockTimer = 0.0f;
	float lockDelay = 0.5f;
	bool isLocking = false;

	// Anti infinite spin / lock delay move limit
	int lockMoveCounter = 0;
	const int maxLockMoves = 10;

	// czy ten klocek już kiedykolwiek dotknął podłoża (dna/innego klocka)
	bool hasTouchedGround = false;

	// helper
	bool isPieceGrounded ();

	Board board;

	sf::Music backgroundMusic;

	// Uwaga: brak konstruktora domyślnego Tetromino, więc inicjalizujemy typem
	Tetromino currentTetromino = Tetromino (TetrominoType::I);

	int lastTetrominoType = -1;

	int startLevel = 0;        // level wybrany w menu (0–9)
	int currentLevel = 0;      // aktualny level w grze
	int totalLinesCleared = 0; // ile linii wyczyszczono od startu

	float getFallSpeedForLevel (int level) const;
	void updateLevelAndSpeed (int linesJustCleared);

	void updateMusicSpeed ();

	

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

  // Metody pomocnicze
  bool checkCollision();
  void lockTetromino();
  void spawnNewTetromino();
  void loadAudio ();

  // Menu / UI
  void handleMenuSelection();
  void renderGameOver();
};
