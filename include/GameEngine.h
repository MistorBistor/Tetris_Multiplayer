#pragma once
#include <SFML/Graphics.hpp>

#include "Board.h"
#include "Menu.h"
#include "Tetromino.h"
#include "TetrominoType.h"

class GameEngine {
private:
    sf::RenderWindow window;
    sf::Clock clock;

    Menu mainMenu; // Menu główne gry

    bool isRunning = false;

    // Spadanie klocka
    float fallTimer = 0.0f;
    float fallSpeed = 0.5f; // Im mniejsza wartość, tym szybciej spada klocek

    // Lock delay: klocek "czeka" chwilę po dotknięciu dna zanim się zablokuje
    float lockTimer = 0.0f;
    float lockDelay = 0.5f;
    bool isLocking = false;

    enum class GameState { Menu, Playing, Paused, GameOver };
    GameState gameState = GameState::Menu;

    Board board;

    // Uwaga: brak konstruktora domyślnego Tetromino, więc inicjalizujemy typem
    Tetromino currentTetromino = Tetromino (TetrominoType::I);

public:
    GameEngine ();
    void initialize ();
    void run ();
    void shutdown ();

private:
    void handleEvents ();
    void update (float deltaTime);
    void render ();

    void handleKeyPress (sf::Keyboard::Key key);

    // Metody pomocnicze
    bool checkCollision ();
    void lockTetromino ();
    void spawnNewTetromino ();

    // Menu / UI
    void handleMenuSelection ();
    void renderGameOver ();
};