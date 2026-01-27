#include "GameEngine.h"

#include <ctime>
#include <iostream>
#include <random>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <vector>

GameEngine::GameEngine() : window(sf::VideoMode(800, 700), "Tetris") {}

void GameEngine::initialize() {
    isRunning = true;
    gameState = GameState::Menu;

    mainMenu.initialize();
    mainMenu.setState(MenuState::MAIN_MENU);
  
    score.initialize();
    score.reset();

    playerName = "";
    selectedGameOverElement = 0;
    isTypingName = false;
  
    if (!uiFont.loadFromFile("../resources/fonts/SourceSansPro-Regular.otf")) {
        std::cout << "[GameEngine ERROR] Nie udało się załadować czcionki UI!\n";
    }
  
    nextQueue.clear();
    for (int i = 0; i < 5; i++) {
    nextQueue.push_back(getRandomTetrominoType());
  }
  
    heldTetrominoType = TetrominoType::Empty;
    hasHeldPiece = false;
    canHold = true;

    currentTetromino = Tetromino(TetrominoType::I);

    loadAudio ();

    fallTimer = 0.0f;
    lockTimer = 0.0f;
    isLocking = false;

    hasTouchedGround = false;
    lockMoveCounter = 0;

    std::cout << "[GameEngine] Initialize -> Menu\n";
}

void GameEngine::run() {
  // Sprawdź czy kontroler jest podłączony
  if (sf::Joystick::isConnected(0)) {
    std::cout << "[GameEngine] Kontroler Xbox wykryty!\n";
  }

  while(window.isOpen()) {
    float deltaTime = clock.restart().asSeconds();

    handleEvents ();

    if (gameState == GameState::Playing) {
      update (deltaTime);
    }

    render();
  }
}

void GameEngine::handleEvents () {
    sf::Event event;
    while (window.pollEvent (event)) {
        if (event.type == sf::Event::Closed) {
            std::cout << "[GameEngine] Window closed\n";
            window.close ();
            return;
        }

        // OBSŁUGA KONTROLERA
        if (event.type == sf::Event::JoystickButtonPressed) {
            if (gameState == GameState::Menu) {
                handleControllerButtonMenu(event.joystickButton.button);
            } else if (gameState == GameState::Playing) {
                handleControllerButton(event.joystickButton.button);
            } else if (gameState == GameState::GameOver) {
                // Obsługa kontrolera w Game Over
                if (event.joystickButton.button == 0) {  // A - Potwierdź
                    if (selectedGameOverElement == 1) {  // Confirm
                        saveHighScore();
                        board.reset();
                        mainMenu.setState(MenuState::MAIN_MENU);
                        gameState = GameState::Menu;
                    } else if (selectedGameOverElement == 2) {  // Main Menu
                        board.reset();
                        mainMenu.setState(MenuState::MAIN_MENU);
                        gameState = GameState::Menu;
                    } else if (selectedGameOverElement == 0) {  // Input field
                        isTypingName = !isTypingName;
                    }
                    sfxMenu.play();
                }
            }
        }
        
        if (event.type == sf::Event::JoystickMoved) {
            if (gameState == GameState::Menu) {
                handleControllerAxisMenu(event.joystickMove.axis, event.joystickMove.position);
            } else if (gameState == GameState::Playing) {
                handleControllerAxis(event.joystickMove.axis, event.joystickMove.position);
            } else if (gameState == GameState::GameOver) {
                // Nawigacja kontrolerem w Game Over
                static bool upPressed = false;
                static bool downPressed = false;
                const float threshold = 50.0f;
                
                if (event.joystickMove.axis == sf::Joystick::PovY) {
                    if (event.joystickMove.position > threshold && !upPressed) {
                        selectedGameOverElement--;
                        if (selectedGameOverElement < 0) selectedGameOverElement = 2;
                        sfxMenu.play();
                        upPressed = true;
                    } else if (event.joystickMove.position < -threshold && !downPressed) {
                        selectedGameOverElement++;
                        if (selectedGameOverElement > 2) selectedGameOverElement = 0;
                        sfxMenu.play();
                        downPressed = true;
                    } else if (event.joystickMove.position > -threshold && event.joystickMove.position < threshold) {
                        upPressed = false;
                        downPressed = false;
                    }
                }
            }
        }

        if (gameState == GameState::Menu) {
            mainMenu.handleEvent (event);

            if (mainMenu.getState () == MenuState::DIFFICULTY_SELECTION) {
                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left) {

                    int clickResult = mainMenu.checkDifficultyClick (
                        static_cast<float>(event.mouseButton.x),
                        static_cast<float>(event.mouseButton.y));

                    if (clickResult == 1) {
                        std::cout << "[Menu] Click -> Confirm\n";
                        handleMenuSelection ();
                    }
                    else if (clickResult == 2) {
                        std::cout << "[Menu] Click -> Back\n";
                        mainMenu.setState (MenuState::MAIN_MENU);
                    }
                }

                if (event.type == sf::Event::KeyPressed &&
                    event.key.code == sf::Keyboard::Enter) {
                    std::cout << "[Menu] Enter -> Confirm\n";
                    handleMenuSelection ();
                }
            }
            else {
                if (event.type == sf::Event::KeyPressed &&
                    event.key.code == sf::Keyboard::Enter) {
                    std::cout << "[Menu] Enter -> Select\n";
                    handleMenuSelection ();
                }

                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left) {
                    std::cout << "[Menu] Mouse click -> Select\n";
                    handleMenuSelection ();
                }
            }

            continue;
        }

        if (gameState == GameState::Playing) {
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    std::cout << "[GameEngine] Pause -> Menu\n";
                    mainMenu.setState (MenuState::PAUSE);
                    gameState = GameState::Menu;

                    backgroundMusic.pause ();
                }
                else {
                    if (board.isLineClearAnimating ()) {
                        continue;
                    }

                    handleKeyPress (event.key.code);
                }
            }
        }

        // GAME OVER
        if (gameState == GameState::GameOver) {
            handleGameOverInput(event);
            continue;
        }
    }
}

void GameEngine::update(float deltaTime) {
    // Ciągłe sprawdzanie D-pada dla soft drop (kontroler)
    if (sf::Joystick::isConnected(0)) {
        float povY = sf::Joystick::getAxisPosition(0, sf::Joystick::PovY);
        if (povY < -50.0f) {  // D-pad w dół (wartości są odwrócone)
            // Soft drop - szybsze opadanie
            static float softDropTimer = 0.0f;
            softDropTimer += deltaTime;
            
            const float softDropSpeed = 0.05f;  // Bardzo szybko
            if (softDropTimer >= softDropSpeed) {
                softDropTimer = 0.0f;
                currentTetromino.moveDown();
                
                if (checkCollision()) {
                    currentTetromino.moveUp();
                    if (!isLocking) {
                        isLocking = true;
                        lockTimer = 0.0f;
                        hasTouchedGround = true;
                    }
                    if (sfxLanded.getStatus() != sf::Sound::Playing)
                        sfxLanded.play();
                }
            }
        }
    }

    // Animacja czyszczenia linii
    if (board.isLineClearAnimating()) {
        int cleared = board.updateClearAnimation(deltaTime);
        if (cleared > 0) {
            std::cout << "[GameEngine] Cleared lines (after anim): " << cleared << "\n";
            
            // DODANE: Dodaj score i zwiększ level
            score.addScore(cleared, score.getLevel());
            updateLevelAndSpeed(cleared);
            
            spawnNewTetromino();
        }
        return;
    }

    // Jeśli nie było animacji czyszczenia, resetuj combo
    static bool wasClearing = false;
    if (wasClearing && !board.isLineClearAnimating()) {
        // Animacja się skończyła, ale nie było linii
        score.resetCombo();
        wasClearing = false;
    }
    if (board.isLineClearAnimating()) {
        wasClearing = true;
    }

    fallTimer += deltaTime;

    if (fallTimer >= fallSpeed) {
        fallTimer = 0.0f;

        std::cout << "[GameEngine] Falling, y=" << currentTetromino.getY() << "\n";
        currentTetromino.moveDown();

        if (checkCollision()) {
            currentTetromino.moveUp();

            if (!hasTouchedGround) {
                hasTouchedGround = true;
                lockMoveCounter = 0;
                std::cout << "[LockDelay] First touch -> start counting moves\n";
            }

            if (!isLocking) {
                std::cout << "[GameEngine] Start lock delay\n";
                isLocking = true;
                lockTimer = 0.0f;

                if (sfxLanded.getStatus() != sf::Sound::Playing)
                    sfxLanded.play();
            }
        }
        else {
            if (isLocking) {
                std::cout << "[GameEngine] Cancel lock delay\n";
                isLocking = false;
                lockTimer = 0.0f;
            }
        }
    }

    if (isLocking) {
        if (!isPieceGrounded()) {
            isLocking = false;
            lockTimer = 0.0f;
            return;
        }

        if (hasTouchedGround && lockMoveCounter >= maxLockMoves) {
            std::cout << "[LockDelay] Max moves reached -> force lock\n";
            lockTetromino();

            if (board.startClearAnimation()) {
                sfxLineClear.play();
            }
            else {
                // Jeśli nie ma linii do wyczyszczenia, resetuj combo
                score.resetCombo();
                spawnNewTetromino();
            }
            return;
        }

        lockTimer += deltaTime;

        if (lockTimer >= lockDelay) {
            std::cout << "[GameEngine] Lock piece (time)\n";
            lockTetromino();

            if (board.startClearAnimation()) {
                sfxLineClear.play();
            }
            else {
                // Jeśli nie ma linii do wyczyszczenia, resetuj combo
                score.resetCombo();
                spawnNewTetromino();
            }
            return;
        }
    }
}

void GameEngine::render() {
    window.clear (sf::Color::White);

    if (gameState == GameState::Menu) {
        mainMenu.render (window);
    }
    else if (gameState == GameState::Playing) {
        board.render (window);

        if (!board.isLineClearAnimating ()) {
            renderGhostTetromino ();
            currentTetromino.render (window);
        }
        
        // Renderuj UI
        score.render(window);
        renderNextPiece(window);
        renderHeldPiece(window);
    }
    else if (gameState == GameState::GameOver) {
        board.render (window);
        score.render(window);
        renderNextPiece(window);
        renderHeldPiece(window);
        renderGameOver ();
    }

    window.display ();
}

void GameEngine::shutdown() { 
    std::cout << "[GameEngine] Shutdown\n"; 
}

bool GameEngine::checkCollision () {
    const auto& shape = currentTetromino.getShape ();
    int tetrominoX = currentTetromino.getX ();
    int tetrominoY = currentTetromino.getY ();

    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            if (shape[row][col] == 1) {
                int boardX = tetrominoX + col;
                int boardY = tetrominoY + row;

                if (!board.isValidPosition (boardX, boardY)) {
                    return true;
                }
            }
        }
    }

    return false;
}

void GameEngine::lockTetromino () {
    const auto& shape = currentTetromino.getShape ();
    int tetrominoX = currentTetromino.getX ();
    int tetrominoY = currentTetromino.getY ();

    board.lockTetromino (tetrominoX, tetrominoY, shape, currentTetromino.getType ());
    std::cout << "[GameEngine] Locked at (" << tetrominoX << ", " << tetrominoY << ")\n";
}

void GameEngine::spawnNewTetromino () {
    std::cout << "[GameEngine] Spawn new tetromino (soft random)\n";

    static std::mt19937 generator (static_cast<unsigned int>(std::time (nullptr)));
    std::uniform_int_distribution<int> distribution (0, 6);

    int randomType;

    do {
        randomType = distribution (generator);
    } while (randomType == lastTetrominoType);

    lastTetrominoType = randomType;

  // USUNIĘTE: Nie czyścimy linii tutaj, zrobi to animacja
  // int linesCleared = board.clearFullLines();
  // if (linesCleared > 0) {
  //   score.addScore(linesCleared, score.getLevel());
  // } else {
  //   score.resetCombo();
  // }
}

/**
 * Generuje losowy typ klocka (I, O, T, S, Z, J, L).
 */
TetrominoType GameEngine::getRandomTetrominoType() {
  static std::mt19937 generator(static_cast<unsigned int>(std::time(nullptr)));
  std::uniform_int_distribution<int> distribution(0, 6);
  
  int randomType = distribution(generator);
  return static_cast<TetrominoType>(randomType);
}

void GameEngine::spawnNewTetromino() {
  std::cout << "[GameEngine] Spawn new tetromino\n";

  isLocking = false;
  lockTimer = 0.0f;
  lockMoveCounter = 0;
  hasTouchedGround = false;
  
  // Pobierz pierwszy klocek z kolejki
  currentTetromino = Tetromino(nextQueue[0]);
  std::cout << "[GameEngine] Spawned type=" << static_cast<int>(nextQueue[0]) << "\n";
  
  // Przesuń kolejkę i dodaj nowy na końcu
  nextQueue.erase(nextQueue.begin());
  nextQueue.push_back(getRandomTetrominoType());
  
  canHold = true;

  if (checkCollision()) {
    std::cout << "[GameEngine] GAME OVER - no space for new piece\n";
    backgroundMusic.stop();
    sfxGameOver.play();
    gameState = GameState::GameOver;

    //Reset zmiennych Game Over
    playerName = "";
    selectedGameOverElement = 0;
    isTypingName = false;
  }
}

void GameEngine::loadAudio () {
    std::cout << "[Audio] CWD = " << std::filesystem::current_path ().string () << "\n";

    const std::string path = "../resources/audio/Original_Tetris_theme.ogg";

    std::cout << "[Audio] Exists? " << std::filesystem::exists (path) << " path=" << path << "\n";

    if (!backgroundMusic.openFromFile (path)) {
        std::cout << "[ERROR] Nie udało się załadować muzyki!\n";
        return;
    }

    backgroundMusic.setLoop (true);
    backgroundMusic.setVolume (50.f);
    std::cout << "[Audio] Muzyka załadowana, duration="
        << backgroundMusic.getDuration ().asSeconds () << "s\n";

    auto loadSfx = [&](sf::SoundBuffer& buf, sf::Sound& snd, const std::string& p) {
        std::cout << "[Audio] SFX load: " << p
            << " exists=" << std::filesystem::exists (p) << "\n";

        if (!buf.loadFromFile (p)) {
            std::cout << "[ERROR] Nie udało się załadować SFX: " << p << "\n";
            return false;
        }
        snd.setBuffer (buf);
        snd.setVolume (sfxVolume);
        return true;
        };

    // Ścieżki do SFX (dopasowane do Twoich nazw)
    loadSfx (sfxMenuBuf, sfxMenu, "../resources/audio/Tetris_menu_sound.ogg");
    loadSfx (sfxMoveBuf, sfxMove, "../resources/audio/Tetris_move_piece.ogg");
    loadSfx (sfxRotateBuf, sfxRotate, "../resources/audio/Tetris_rotate_piece.ogg");
    loadSfx (sfxLandedBuf, sfxLanded, "../resources/audio/Tetris_piece_landed.ogg");
    loadSfx (sfxLineClearBuf, sfxLineClear, "../resources/audio/Tetris_line_clear.ogg");
    loadSfx (sfxGameOverBuf, sfxGameOver, "../resources/audio/Tetris_game_over.ogg");
}

void GameEngine::handleMenuSelection() {
  MenuAction action = mainMenu.handleSelection();
  std::cout << "[GameEngine] Menu action=" << static_cast<int>(action) << "\n";

  if (action != MenuAction::NONE) {
    sfxMenu.play();
  }

  switch (action) {
  case MenuAction::START_GAME:
    std::cout << "[Menu] -> Difficulty selection\n";
    mainMenu.setState(MenuState::DIFFICULTY_SELECTION);
    break;

  case MenuAction::HIGH_SCORES:
    std::cout << "[Menu] -> High Scores\n";
    // TODO: Przejdź do ekranu High Scores
    // Na razie tylko wyświetl komunikat
    std::cout << "[Menu] High Scores - not implemented yet\n";
    break;

  case MenuAction::SETTINGS:
    std::cout << "[Menu] -> Settings\n";
    mainMenu.setState(MenuState::SETTINGS);
    break;

  case MenuAction::CONFIRM_DIFFICULTY:
    std::cout << "[Menu] Start game\n";
    fallSpeed = mainMenu.getDifficultySpeed();
    startLevel = mainMenu.getSelectedDifficulty();
    currentLevel = startLevel;
    totalLinesCleared = 0;  // WAŻNE: Reset licznika linii

    fallSpeed = getFallSpeedForLevel(currentLevel);
    updateMusicSpeed();
    board.updateClearAnimSpeed(currentLevel);

    board.reset();
    score.reset();
    
    // Ustaw level w Score zgodnie z wybraną trudnością
    for (int i = 0; i < startLevel; i++) {
        score.increaseLevel();
    }
    
    nextQueue.clear();
    for (int i = 0; i < 5; i++) {
        nextQueue.push_back(getRandomTetrominoType());
    }
    heldTetrominoType = TetrominoType::Empty;
    hasHeldPiece = false;
    canHold = true;
    
    gameState = GameState::Playing;
    spawnNewTetromino();
    backgroundMusic.play();
    std::cout << "[Audio] Start muzyki\n";
    break;

  case MenuAction::RESUME:
    std::cout << "[Menu] Resume\n";
    gameState = GameState::Playing;
    backgroundMusic.play();
    break;

  case MenuAction::RESTART:
    std::cout << "[Menu] Restart\n";
    board.reset();
    mainMenu.setState(MenuState::DIFFICULTY_SELECTION);
    gameState = GameState::Menu;
    backgroundMusic.stop();
    break;

  case MenuAction::MAIN_MENU:
    std::cout << "[Menu] Main menu\n";
    board.reset();
    mainMenu.setState(MenuState::MAIN_MENU);
    gameState = GameState::Menu;
    backgroundMusic.stop();
    break;

  case MenuAction::EXIT:
    std::cout << "[Menu] Exit\n";
    backgroundMusic.stop();
    window.close();
    break;

        board.reset ();
        gameState = GameState::Playing;
        spawnNewTetromino ();

        backgroundMusic.play ();
        std::cout << "[Audio] Start muzyki\n";
        break;

    case MenuAction::RESUME:
        std::cout << "[Menu] Resume\n";
        gameState = GameState::Playing;

        backgroundMusic.play ();

        break;

    case MenuAction::RESTART:
        std::cout << "[Menu] Restart\n";
        board.reset ();
        mainMenu.setState (MenuState::DIFFICULTY_SELECTION);
        gameState = GameState::Menu;

        backgroundMusic.stop ();
        break;

    case MenuAction::MAIN_MENU:
        std::cout << "[Menu] Main menu\n";
        board.reset ();
        mainMenu.setState (MenuState::MAIN_MENU);
        gameState = GameState::Menu;

        backgroundMusic.stop ();
        break;

    case MenuAction::EXIT:
        std::cout << "[Menu] Exit\n";
        backgroundMusic.stop ();   // 🛑 zatrzymujemy muzykę
        window.close ();           // ❌ zamykamy okno
        break;

    default:
        break;
    }
}

void GameEngine::renderGameOver() {
    sf::RectangleShape overlay(sf::Vector2f(800, 700));
    overlay.setPosition(0, 0);
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    window.draw(overlay);

    // Tytuł GAME OVER
    sf::Text gameOverText;
    gameOverText.setFont(mainMenu.getFont());
    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(56);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setStyle(sf::Text::Bold);

    sf::FloatRect gb = gameOverText.getLocalBounds();
    gameOverText.setOrigin(gb.left + gb.width / 2.f, gb.top + gb.height / 2.f);
    gameOverText.setPosition(400.f, 150.f);
    window.draw(gameOverText);

    // Wynik gracza
    sf::Text scoreText;
    scoreText.setFont(mainMenu.getFont());
    scoreText.setString("Your Score: " + std::to_string(score.getCurrentScore()));
    scoreText.setCharacterSize(32);
    scoreText.setFillColor(sf::Color::Yellow);
    
    sf::FloatRect sb = scoreText.getLocalBounds();
    scoreText.setOrigin(sb.left + sb.width / 2.f, sb.top + sb.height / 2.f);
    scoreText.setPosition(400.f, 220.f);
    window.draw(scoreText);

    // Input field dla nazwy
    sf::RectangleShape inputBox(sf::Vector2f(200, 40));
    inputBox.setPosition(300, 280);
    inputBox.setFillColor(sf::Color(40, 40, 60));  // Ciemny niebieski
    inputBox.setOutlineThickness(2);
    inputBox.setOutlineColor(selectedGameOverElement == 0 ? sf::Color::Yellow : sf::Color::White);
    window.draw(inputBox);

    // Tekst "Enter Name:"
    sf::Text nameLabel;
    nameLabel.setFont(mainMenu.getFont());
    nameLabel.setString("Enter Name:");
    nameLabel.setCharacterSize(20);
    nameLabel.setFillColor(sf::Color::White);
    nameLabel.setPosition(310, 260);
    window.draw(nameLabel);

    // Wpisana nazwa (lub placeholder)
    sf::Text nameText;
    nameText.setFont(mainMenu.getFont());
    nameText.setString(playerName.empty() ? (isTypingName ? "_" : "Anonymous") : playerName + (isTypingName ? "_" : ""));
    nameText.setCharacterSize(24);
    nameText.setFillColor(sf::Color::White);
    nameText.setPosition(310, 285);
    window.draw(nameText);
   
    // Przycisk Main Menu
    sf::RectangleShape menuButton(sf::Vector2f(100, 40));
    menuButton.setPosition(290, 350);
    menuButton.setFillColor(selectedGameOverElement == 2 ? sf::Color(200, 50, 50) : sf::Color(120, 30, 30));
    menuButton.setOutlineThickness(2);
    menuButton.setOutlineColor(selectedGameOverElement == 2 ? sf::Color::Yellow : sf::Color::White);
    window.draw(menuButton);

    sf::Text menuText;
    menuText.setFont(mainMenu.getFont());
    menuText.setString("Main Menu");
    menuText.setCharacterSize(18);
    menuText.setFillColor(sf::Color::White);
    
    // wyswordkowanie tekstu Main Menu
    sf::FloatRect menuBounds = menuText.getLocalBounds();
    menuText.setOrigin(menuBounds.left + menuBounds.width / 2.f, menuBounds.top + menuBounds.height / 2.f);
    menuText.setPosition(340.f, 370.f);  // Środek przycisku: 290 + 100/2 = 340
    window.draw(menuText);

    // Przycisk Confirm
    sf::RectangleShape confirmButton(sf::Vector2f(100, 40));
    confirmButton.setPosition(410, 350);
    confirmButton.setFillColor(selectedGameOverElement == 1 ? sf::Color(50, 200, 50) : sf::Color(30, 120, 30));
    confirmButton.setOutlineThickness(2);
    confirmButton.setOutlineColor(selectedGameOverElement == 1 ? sf::Color::Yellow : sf::Color::White);
    window.draw(confirmButton);

    sf::Text confirmText;
    confirmText.setFont(mainMenu.getFont());
    confirmText.setString("Confirm");
    confirmText.setCharacterSize(20);
    confirmText.setFillColor(sf::Color::White);
    
    // wyswordkowanie tekstu Confirm
    sf::FloatRect confirmBounds = confirmText.getLocalBounds();
    confirmText.setOrigin(confirmBounds.left + confirmBounds.width / 2.f, confirmBounds.top + confirmBounds.height / 2.f);
    confirmText.setPosition(460.f, 370.f);  // Środek przycisku: 410 + 100/2 = 460
    window.draw(confirmText);

    // Instrukcja
    sf::Text infoText;
    infoText.setFont(mainMenu.getFont());
    infoText.setString("Use arrows to navigate, Enter to select");
    infoText.setCharacterSize(16);
    infoText.setFillColor(sf::Color(150, 150, 150));
    
    sf::FloatRect ib = infoText.getLocalBounds();
    infoText.setOrigin(ib.left + ib.width / 2.f, ib.top + ib.height / 2.f);
    infoText.setPosition(400.f, 420.f);
    window.draw(infoText);
}

void GameEngine::renderGhostTetromino () {
    Tetromino ghost = currentTetromino;

    while (true) {
        ghost.moveDown ();

        const auto& shape = ghost.getShape ();
        int gx = ghost.getX ();
        int gy = ghost.getY ();

        bool coll = false;
        for (int row = 0; row < 4 && !coll; row++) {
            for (int col = 0; col < 4; col++) {
                if (shape[row][col] == 1) {
                    int boardX = gx + col;
                    int boardY = gy + row;
                    if (!board.isValidPosition (boardX, boardY)) {
                        coll = true;
                        break;
                    }
                }
            }
        }

        if (coll) {
            ghost.moveUp ();
            break;
        }
    }

    if (ghost.getY () == currentTetromino.getY ()) {
        return;
    }

    ghost.renderGhost (window);
}

void GameEngine::handleKeyPress (sf::Keyboard::Key key) {
    auto countLockMoveIfNeeded = [&]() {
        if (hasTouchedGround) {
            lockMoveCounter++;
            std::cout << "[LockDelay] Move counter = " << lockMoveCounter << "\n";
        }
    };

    auto resetLockTimerIfGrounded = [&]() {
        if (hasTouchedGround && isPieceGrounded ()) {
            lockTimer = 0.0f;
        }
    };

    if (key == sf::Keyboard::H) {
        std::cout << "[Input] H (hold)\n";
        holdCurrentPiece();
        return;
    }
    
    if (key == sf::Keyboard::Left) {
        std::cout << "[Input] LEFT\n";
        currentTetromino.moveLeft ();

        if (checkCollision ()) {
            std::cout << "[Input] Collision -> undo LEFT\n";
            currentTetromino.moveRight ();
        }
        else {
            countLockMoveIfNeeded ();
            resetLockTimerIfGrounded ();
            if (sfxMove.getStatus () != sf::Sound::Playing)
                sfxMove.play ();
        }
    }
    else if (key == sf::Keyboard::Right) {
        std::cout << "[Input] RIGHT\n";
        currentTetromino.moveRight ();

        if (checkCollision ()) {
            std::cout << "[Input] Collision -> undo RIGHT\n";
            currentTetromino.moveLeft ();
        }
        else {
            countLockMoveIfNeeded ();
            resetLockTimerIfGrounded ();
            if (sfxMove.getStatus () != sf::Sound::Playing)
                sfxMove.play ();
        }
    }
    else if (key == sf::Keyboard::Down) {
        std::cout << "[Input] DOWN\n";
        currentTetromino.moveDown ();

        if (checkCollision ()) {
            currentTetromino.moveUp ();

            if (!isLocking) {
                isLocking = true;
                lockTimer = 0.0f;
                hasTouchedGround = true;
            }
            if (sfxLanded.getStatus () != sf::Sound::Playing)
                sfxLanded.play ();
        }
    }
    else if (key == sf::Keyboard::Space) {
        std::cout << "[Input] SPACE (hard drop)\n";
        while (!checkCollision()) {
            currentTetromino.moveDown();
        }
        currentTetromino.moveUp();
        lockTetromino();
        if (sfxLanded.getStatus() != sf::Sound::Playing)
            sfxLanded.play();

        // Sprawdź czy są linie do animacji
        if (board.startClearAnimation()) {
            sfxLineClear.play();
        }
        else {
            score.resetCombo();
            spawnNewTetromino();
        
            // Jeśli spawn spowodował Game Over, nie rób nic więcej
            if (gameState == GameState::GameOver) {
                return;
            }
        }
}
    else if (key == sf::Keyboard::Up) {
        std::cout << "[Input] Gracz naciska: GÓRA (rotacja + wall kick)\n";

        const int startX = currentTetromino.getX ();
        const int startY = currentTetromino.getY ();

        currentTetromino.rotate ();

        const std::vector<std::pair<int, int>> kickTests = {
          {0, 0},
          {-1, 0}, {1, 0},
          {-2, 0}, {2, 0},
          {0, -1},
          {-1, -1}, {1, -1},
          {0, -2}
        };

        auto moveBy = [&](int dx, int dy) {
            if (dx > 0)
                for (int i = 0; i < dx; i++) currentTetromino.moveRight ();
            else if (dx < 0)
                for (int i = 0; i < -dx; i++) currentTetromino.moveLeft ();

            if (dy > 0)
                for (int i = 0; i < dy; i++) currentTetromino.moveDown ();
            else if (dy < 0)
                for (int i = 0; i < -dy; i++) currentTetromino.moveUp ();
        };

        auto resetToStartPos = [&]() {
            moveBy (startX - currentTetromino.getX (),
                startY - currentTetromino.getY ());
        };

        bool rotationPlaced = false;

        for (size_t i = 0; i < kickTests.size (); i++) {
            resetToStartPos ();

            const int kickX = kickTests[i].first;
            const int kickY = kickTests[i].second;

            moveBy (kickX, kickY);

            if (!checkCollision ()) {
                std::cout << "[WallKick] Rotacja OK, test=" << i
                    << " kick=(" << kickX << "," << kickY << ")\n";

                rotationPlaced = true;
                sfxRotate.play ();

                const bool groundedAfter = isPieceGrounded ();

                countLockMoveIfNeeded ();

                if (groundedAfter) {
                    lockTimer = 0.0f;
                    std::cout << "[WallKick] Reset lockTimer po rotacji\n";
                }

                break;
            }
        }

        if (!rotationPlaced) {
            std::cout << "[WallKick] Rotacja zablokowana – cofanie\n";

            resetToStartPos ();

            currentTetromino.rotate ();
            currentTetromino.rotate ();
            currentTetromino.rotate ();
        }
    }
}


bool GameEngine::isPieceGrounded() {
    currentTetromino.moveDown ();
    const bool coll = checkCollision ();
    currentTetromino.moveUp ();
    return coll;
}

float GameEngine::getFallSpeedForLevel(int level) const {
    // Twoje levele 0–9 (dokładnie to, co masz w Menu)
    static const int framesPerDrop[] = {
      48, 43, 38, 33, 28, 23, 18, 13, 8, 6
    };

    int frames;

    if (level <= 9) {
        frames = framesPerDrop[level];
    }
    else {
        // Po levelu 9 dalej przyspieszamy
        frames = std::max (1, 6 - (level - 9));
    }

    return frames / 60.0f;
}

void GameEngine::updateLevelAndSpeed(int linesJustCleared) {
    totalLinesCleared += linesJustCleared;

    int oldLevel = currentLevel;
    currentLevel = startLevel + (totalLinesCleared / 10);  // Co 10 linii = +1 level

    fallSpeed = getFallSpeedForLevel(currentLevel);

    std::cout << "[LEVEL] start=" << startLevel
              << " lines=" << totalLinesCleared
              << " level=" << currentLevel
              << " fallSpeed=" << fallSpeed << "\n";

    // Aktualizuj level w Score
    while (score.getLevel() < currentLevel) {
        score.increaseLevel();
    }

    // Przyspiesz muzykę tylko gdy level poszedł w górę
    if (currentLevel != oldLevel) {
        updateMusicSpeed();
        board.updateClearAnimSpeed(currentLevel);
        std::cout << "[LEVEL UP] " << oldLevel << " -> " << currentLevel << "\n";
    }
}

void GameEngine::updateMusicSpeed() {
    // Bazowe tempo
    const float basePitch = 1.0f;

    // Jak szybko rośnie tempo (0.02–0.05 to dobre wartości)
    const float pitchPerLevel = 0.03f;

    // Maksymalne tempo (żeby nie zwariowało)
    const float maxPitch = 1.6f;

    float pitch = basePitch + currentLevel * pitchPerLevel;
    if (pitch > maxPitch) pitch = maxPitch;

    backgroundMusic.setPitch (pitch);
    std::cout << "[Audio] pitch=" << pitch << " level=" << currentLevel << "\n";
}

/**
 * Zatrzymuje aktualny klocek i zamienia go z held piece.
 * Jeśli nie ma held piece, aktualny klocek zostaje zatrzymany i spawni się nowy.
 */
void GameEngine::holdCurrentPiece() {
  if (!canHold) {
    std::cout << "[GameEngine] Hold już użyty w tej turze!\n";
    return;
  }
  
  std::cout << "[GameEngine] Hold piece\n";
  
  TetrominoType currentType = currentTetromino.getType();
  
  if (hasHeldPiece) {
    currentTetromino = Tetromino(heldTetrominoType);
    heldTetrominoType = currentType;
  } else {
    heldTetrominoType = currentType;
    hasHeldPiece = true;
    
    // ZMIENIONE: Spawnuj z kolejki
    currentTetromino = Tetromino(nextQueue[0]);
    nextQueue.erase(nextQueue.begin());
    nextQueue.push_back(getRandomTetrominoType());
  }
  
  canHold = false;
  fallTimer = 0.0f;
  isLocking = false;
  lockTimer = 0.0f;
}

/**
 * Pomocnicza funkcja do renderowania podglądu klocka.
 */
void GameEngine::renderTetrominoPreview(sf::RenderWindow& window, TetrominoType type, float x, float y) const {
  if (type == TetrominoType::Empty) {
    return;
  }
  
  const int CELL_SIZE = 16;  // ZMIENIONE z 20 na 18 (mniejsze klocki)
  
  sf::Color color;
  std::vector<std::vector<int>> shape;
  
  switch (type) {
    case TetrominoType::I:
      color = sf::Color::Cyan;
      shape = {{0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 0, 0}};
      break;
    case TetrominoType::O:
      color = sf::Color::Yellow;
      shape = {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}};
      break;
    case TetrominoType::T:
      color = sf::Color::Magenta;
      shape = {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 1, 0}, {0, 0, 0, 0}};
      break;
    case TetrominoType::S:
      color = sf::Color::Green;
      shape = {{0, 0, 0, 0}, {0, 0, 1, 1}, {0, 1, 1, 0}, {0, 0, 0, 0}};
      break;
    case TetrominoType::Z:
      color = sf::Color::Red;
      shape = {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 0}};
      break;
    case TetrominoType::J:
      color = sf::Color::Blue;
      shape = {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 0, 0, 1}, {0, 0, 0, 0}};
      break;
    case TetrominoType::L:
      color = sf::Color(255, 165, 0);
      shape = {{0, 0, 0, 0}, {0, 1, 1, 1}, {0, 1, 0, 0}, {0, 0, 0, 0}};
      break;
    default:
      return;
  }
  
  sf::RectangleShape block(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
  block.setFillColor(color);
  
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      if (shape[row][col] == 1) {
        block.setPosition(x + col * CELL_SIZE, y + row * CELL_SIZE);
        window.draw(block);
      }
    }
  }
}


/**
 * Renderuje panel "HOLD" z zatrzymanym klockiem.
 * Pozycja: na wysokości Score (góra ekranu, lewa strona)
 */
void GameEngine::renderHeldPiece(sf::RenderWindow& window) const {
  const float leftMargin = 60.0f;
  
  sf::RectangleShape panel(sf::Vector2f(150, 140));
  panel.setPosition(leftMargin, 50);
  panel.setFillColor(sf::Color(50, 50, 50));
  panel.setOutlineThickness(2);
  panel.setOutlineColor(sf::Color::White);
  window.draw(panel);
  
  sf::Text titleText;
  titleText.setFont(uiFont);
  titleText.setString("HOLD");
  titleText.setCharacterSize(20);
  titleText.setFillColor(sf::Color::Magenta);
  titleText.setPosition(leftMargin + 20, 60);
  window.draw(titleText);
  
  if (hasHeldPiece) {
    renderTetrominoPreview(window, heldTetrominoType, leftMargin + 30, 100);
  }
  
  if (!canHold) {
    sf::RectangleShape overlay(sf::Vector2f(150, 140));
    overlay.setPosition(leftMargin, 50);
    overlay.setFillColor(sf::Color(0, 0, 0, 100));
    window.draw(overlay);
  }
}

/**
 * Renderuje panel "NEXT" z 4 następnymi klockami.
 * Wyrównany dolną granicą z dolną granicą siatki (y=650)
 */
void GameEngine::renderNextPiece(sf::RenderWindow& window) const {
  const float leftMargin = 60.0f;
  const float topPosition = 210.0f;  // Pod Hold
  
  // Siatka kończy się na y=650 (50 + 20*30)
  // Next: wysokość = 650 - 210 = 440
  const float panelHeight = 440.0f;
  
  sf::RectangleShape panel(sf::Vector2f(150, panelHeight));
  panel.setPosition(leftMargin, topPosition);
  panel.setFillColor(sf::Color(50, 50, 50));
  panel.setOutlineThickness(2);
  panel.setOutlineColor(sf::Color::White);
  window.draw(panel);
  
  sf::Text titleText;
  titleText.setFont(uiFont);
  titleText.setString("NEXT");
  titleText.setCharacterSize(20);
  titleText.setFillColor(sf::Color::Green);
  titleText.setPosition(leftMargin + 20, topPosition + 10);
  window.draw(titleText);
  
  // Renderuj 4 następne klocki (zmniejszone z 5)
  // Spacing: (440 - 50 dla tytułu) / 4 ≈ 97px na klocek
  for (size_t i = 0; i < nextQueue.size() && i < 4; i++) {
    float yPos = topPosition + 50 + i * 97;
    renderTetrominoPreview(window, nextQueue[i], leftMargin + 30, yPos);
  }
}

/**
 * Obsługa przycisków kontrolera Xbox.
 * Mapowanie Xbox Series X:
 * 0 = A, 1 = B, 2 = X, 3 = Y
 * 4 = LB, 5 = RB, 6 = Back, 7 = Start
 */
void GameEngine::handleControllerButton(unsigned int button) {
  std::cout << "[Controller] Button pressed: " << button << "\n";
  
  switch (button) {
    case 3:  // Y - Rotacja
      handleKeyPress(sf::Keyboard::Up);
      break;
      
    case 2:  // X - Hard Drop
      handleKeyPress(sf::Keyboard::Space);
      break;
      
    case 1:  // B - Hold
      holdCurrentPiece();
      break;
      
    case 7:  // Start/Menu - Pauza
      std::cout << "[Controller] Start (pause)\n";
      mainMenu.setState(MenuState::PAUSE);
      gameState = GameState::Menu;
      backgroundMusic.pause();  // DODANE: pauzowanie muzyki
      break;
  }
}

/**
 * Obsługa D-pada kontrolera Xbox.
 * Axis 6 (PovX) i 7 (PovY) to D-pad.
 */
void GameEngine::handleControllerAxis(sf::Joystick::Axis axis, float position) {
  static bool dpadLeftPressed = false;
  static bool dpadRightPressed = false;
  static bool dpadDownPressed = false;
  
  const float threshold = 50.0f;
  
  // D-pad lewo/prawo (Axis::PovX)
  if (axis == sf::Joystick::PovX) {
    if (position < -threshold && !dpadLeftPressed) {
      std::cout << "[Controller] D-pad LEFT\n";
      handleKeyPress(sf::Keyboard::Left);
      dpadLeftPressed = true;
    } else if (position > threshold && !dpadRightPressed) {
      std::cout << "[Controller] D-pad RIGHT\n";
      handleKeyPress(sf::Keyboard::Right);
      dpadRightPressed = true;
    } else if (position > -threshold && position < threshold) {
      dpadLeftPressed = false;
      dpadRightPressed = false;
    }
  }
  
  // D-pad góra/dół (Axis::PovY) - NAPRAWIONE: Y < 0 to GÓRA, Y > 0 to DÓŁ
  if (axis == sf::Joystick::PovY) {
    // UWAGA: Dla PovY wartości są odwrócone!
    // position < 0 = DÓŁ (down)
    // position > 0 = GÓRA (up)
    
    if (position < -threshold) {
      // DÓŁ - soft drop (ciągłe)
      std::cout << "[Controller] D-pad DOWN (soft drop)\n";
      handleKeyPress(sf::Keyboard::Down);
      dpadDownPressed = true;
    } else {
      dpadDownPressed = false;
    }
}

void GameEngine::updateMusicSpeed () {
    // Bazowe tempo
    const float basePitch = 1.0f;

    // Jak szybko rośnie tempo (0.02–0.05 to dobre wartości)
    const float pitchPerLevel = 0.03f;

    // Maksymalne tempo (żeby nie zwariowało)
    const float maxPitch = 1.6f;

    float pitch = basePitch + currentLevel * pitchPerLevel;
    if (pitch > maxPitch) pitch = maxPitch;

    backgroundMusic.setPitch (pitch);
    std::cout << "[Audio] pitch=" << pitch << " level=" << currentLevel << "\n";
}

/**
 * Obsługa przycisków kontrolera w menu.
 */
void GameEngine::handleControllerButtonMenu(unsigned int button) {
  std::cout << "[Controller Menu] Button: " << button << "\n";
  
  switch (button) {
    case 0:  // A - Potwierdź
      std::cout << "[Controller Menu] A (confirm)\n";
      sfxMenu.play();
      handleMenuSelection();
      break;
      
    case 1:  // B - Back
      std::cout << "[Controller Menu] B (back)\n";
      sfxMenu.play();
      if (mainMenu.getState() == MenuState::DIFFICULTY_SELECTION) {
        mainMenu.setState(MenuState::MAIN_MENU);
      } else if (mainMenu.getState() == MenuState::PAUSE) {
        gameState = GameState::Playing;
        backgroundMusic.play();  // Wznów muzykę przy powrocie do gry
      }
      break;
      
    case 7:  // Start/Menu
      std::cout << "[Controller Menu] Start\n";
      if (mainMenu.getState() == MenuState::PAUSE) {
        gameState = GameState::Playing;
        backgroundMusic.play();  // Wznów muzykę
      }
      break;
  }
}

/**
 * Obsługa osi kontrolera w menu (D-pad i joystick).
 */
void GameEngine::handleControllerAxisMenu(sf::Joystick::Axis axis, float position) {
  static bool upPressed = false;
  static bool downPressed = false;
  static bool leftPressed = false;
  static bool rightPressed = false;
  
  const float threshold = 50.0f;
  
  // D-pad góra/dół (PovY) - odwrócone wartości
  // Lewy joystick (Y) - normalne wartości
  if (axis == sf::Joystick::PovY) {
    // PovY: position > 0 to GÓRA, position < 0 to DÓŁ
    if (position > threshold && !upPressed) {
      std::cout << "[Controller Menu] D-pad UP\n";
      
      if (mainMenu.getState() == MenuState::DIFFICULTY_SELECTION) {
        sf::Event fakeEvent;
        fakeEvent.type = sf::Event::KeyPressed;
        fakeEvent.key.code = sf::Keyboard::Up;
        mainMenu.handleEvent(fakeEvent);
      } else {
        mainMenu.moveUp();
        sfxMenu.play();
      }
      upPressed = true;
    } else if (position < -threshold && !downPressed) {
      std::cout << "[Controller Menu] D-pad DOWN\n";
      
      if (mainMenu.getState() == MenuState::DIFFICULTY_SELECTION) {
        sf::Event fakeEvent;
        fakeEvent.type = sf::Event::KeyPressed;
        fakeEvent.key.code = sf::Keyboard::Down;
        mainMenu.handleEvent(fakeEvent);
      } else {
        mainMenu.moveDown();
        sfxMenu.play();
      }
      downPressed = true;
    } else if (position > -threshold && position < threshold) {
      upPressed = false;
      downPressed = false;
    }
  } else if (axis == sf::Joystick::Y) {
    // Lewy joystick Y: position < 0 to GÓRA, position > 0 to DÓŁ (normalne)
    if (position < -threshold && !upPressed) {
      std::cout << "[Controller Menu] Joystick UP\n";
      
      if (mainMenu.getState() == MenuState::DIFFICULTY_SELECTION) {
        sf::Event fakeEvent;
        fakeEvent.type = sf::Event::KeyPressed;
        fakeEvent.key.code = sf::Keyboard::Up;
        mainMenu.handleEvent(fakeEvent);
      } else {
        mainMenu.moveUp();
        sfxMenu.play();
      }
      upPressed = true;
    } else if (position > threshold && !downPressed) {
      std::cout << "[Controller Menu] Joystick DOWN\n";
      
      if (mainMenu.getState() == MenuState::DIFFICULTY_SELECTION) {
        sf::Event fakeEvent;
        fakeEvent.type = sf::Event::KeyPressed;
        fakeEvent.key.code = sf::Keyboard::Down;
        mainMenu.handleEvent(fakeEvent);
      } else {
        mainMenu.moveDown();
        sfxMenu.play();
      }
      downPressed = true;
    } else if (position > -threshold && position < threshold) {
      upPressed = false;
      downPressed = false;
    }
  }
  
  // D-pad lewo/prawo (PovX) lub lewy joystick (X)
  if (axis == sf::Joystick::PovX || axis == sf::Joystick::X) {
    if (mainMenu.getState() == MenuState::DIFFICULTY_SELECTION) {
      if (position < -threshold && !leftPressed) {
        std::cout << "[Controller Menu] LEFT\n";
        sf::Event fakeEvent;
        fakeEvent.type = sf::Event::KeyPressed;
        fakeEvent.key.code = sf::Keyboard::Left;
        mainMenu.handleEvent(fakeEvent);
        leftPressed = true;
      } else if (position > threshold && !rightPressed) {
        std::cout << "[Controller Menu] RIGHT\n";
        sf::Event fakeEvent;
        fakeEvent.type = sf::Event::KeyPressed;
        fakeEvent.key.code = sf::Keyboard::Right;
        mainMenu.handleEvent(fakeEvent);
        rightPressed = true;
      } else if (position > -threshold && position < threshold) {
        leftPressed = false;
        rightPressed = false;
      }
    }
  }
}

void GameEngine::handleGameOverInput(sf::Event& event) {
    // Wpisywanie nazwy
    if (isTypingName && event.type == sf::Event::TextEntered) {
        if (event.text.unicode == '\b') {  // Backspace
            if (!playerName.empty()) {
                playerName.pop_back();
            }
        } else if (event.text.unicode == '\r' || event.text.unicode == '\n') {  // Enter
            isTypingName = false;
            selectedGameOverElement = 1;  // Przejdź do Confirm
        } else if (event.text.unicode < 128 && playerName.length() < 15) {
            char c = static_cast<char>(event.text.unicode);
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || 
                (c >= '0' && c <= '9') || c == ' ' || c == '_') {
                playerName += c;
            }
        }
        return;  // WAŻNE: Gdy piszemy, nie obsługujemy innych klawiszy
    }

    // Klawiatura - nawigacja
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Up) {
            selectedGameOverElement--;
            if (selectedGameOverElement < 0) selectedGameOverElement = 2;
            isTypingName = false;  // Wyłącz tryb pisania przy nawigacji
            sfxMenu.play();
        } else if (event.key.code == sf::Keyboard::Down) {
            selectedGameOverElement++;
            if (selectedGameOverElement > 2) selectedGameOverElement = 0;
            isTypingName = false;  // Wyłącz tryb pisania przy nawigacji
            sfxMenu.play();
        } else if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right) {
            if (selectedGameOverElement == 1) {
                selectedGameOverElement = 2;
                sfxMenu.play();
            } else if (selectedGameOverElement == 2) {
                selectedGameOverElement = 1;
                sfxMenu.play();
            }
        } else if (event.key.code == sf::Keyboard::Enter) {
            if (selectedGameOverElement == 0) {  // Input field
                isTypingName = !isTypingName;  
                std::cout << "[GameOver] Typing mode: " << (isTypingName ? "ON" : "OFF") << "\n";
            } else if (selectedGameOverElement == 1) {  // Confirm
                saveHighScore();
                board.reset();
                score.reset();
                mainMenu.setState(MenuState::MAIN_MENU);
                gameState = GameState::Menu;
                sfxMenu.play();
            } else if (selectedGameOverElement == 2) {  // Main Menu
                board.reset();
                score.reset();
                mainMenu.setState(MenuState::MAIN_MENU);
                gameState = GameState::Menu;
                sfxMenu.play();
            }
        } else if (event.key.code == sf::Keyboard::Escape) {
            board.reset();
            score.reset();
            mainMenu.setState(MenuState::MAIN_MENU);
            gameState = GameState::Menu;
        }
    }

    // Kliknięcie myszą
    if (event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x),
                             static_cast<float>(event.mouseButton.y));
        
        sf::FloatRect inputBounds(300, 280, 200, 40);
        sf::FloatRect menuBounds(290, 350, 100, 40);
        sf::FloatRect confirmBounds(410, 350, 100, 40);
        
        if (inputBounds.contains(mousePos)) {
            selectedGameOverElement = 0;
            isTypingName = true;
        } else if (confirmBounds.contains(mousePos)) {
            selectedGameOverElement = 1;
            saveHighScore();
            board.reset();
            score.reset();  // DODANE: Reset score
            mainMenu.setState(MenuState::MAIN_MENU);
            gameState = GameState::Menu;
            sfxMenu.play();
        } else if (menuBounds.contains(mousePos)) {
            selectedGameOverElement = 2;
            board.reset();
            score.reset();  // DODANE: Reset score
            mainMenu.setState(MenuState::MAIN_MENU);
            gameState = GameState::Menu;
            sfxMenu.play();
        }
    }
}

void GameEngine::saveHighScore() {
    if (playerName.empty()) {
        playerName = "Anonymous";
    }
    
    std::cout << "[GameEngine] Saving high score: " << playerName 
              << " - " << score.getCurrentScore() << "\n";  // POPRAWIONE
    
    // Wczytaj istniejące wyniki
    std::vector<std::pair<std::string, int>> highScores;
    
    std::ifstream inFile("../resources/high_scores.txt");
    if (inFile.is_open()) {
        std::string name;
        int scoreValue;
        while (inFile >> name >> scoreValue) {
            highScores.push_back({name, scoreValue});
        }
        inFile.close();
    }
    
    // Dodaj nowy wynik
    highScores.push_back({playerName, score.getCurrentScore()});  // POPRAWIONE
    
    // Sortuj malejąco
    std::sort(highScores.begin(), highScores.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Zachowaj tylko top 10
    if (highScores.size() > 10) {
        highScores.resize(10);
    }
    
    // Zapisz do pliku
    std::ofstream outFile("../resources/high_scores.txt");
    if (outFile.is_open()) {
        for (const auto& entry : highScores) {
            outFile << entry.first << " " << entry.second << "\n";
        }
        outFile.close();
        std::cout << "[GameEngine] High scores saved successfully\n";
    } else {
        std::cout << "[GameEngine ERROR] Failed to save high scores\n";
    }
}