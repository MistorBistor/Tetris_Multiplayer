#include "GameEngine.h"

#include <ctime>
#include <iostream>
#include <random>

GameEngine::GameEngine() : window(sf::VideoMode(500, 700), "Tetris") {}

void GameEngine::initialize() {
  isRunning = true;
  gameState = GameState::Menu;

  mainMenu.initialize();
  mainMenu.setState(MenuState::MAIN_MENU);

  currentTetromino = Tetromino(TetrominoType::I);

  fallTimer = 0.0f;
  lockTimer = 0.0f;
  isLocking = false;

  std::cout << "[GameEngine] Initialize -> Menu\n";
}

void GameEngine::run() {
  while (window.isOpen()) {
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
      std::cout << "[GameEngine] Window closed\n";
      window.close();
      return;
    }

    if (gameState == GameState::Menu) {
      mainMenu.handleEvent(event);

      if (mainMenu.getState() == MenuState::DIFFICULTY_SELECTION) {
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left) {

          int clickResult = mainMenu.checkDifficultyClick(
              static_cast<float>(event.mouseButton.x),
              static_cast<float>(event.mouseButton.y));

          if (clickResult == 1) {
            std::cout << "[Menu] Click -> Confirm\n";
            handleMenuSelection();
          } else if (clickResult == 2) {
            std::cout << "[Menu] Click -> Back\n";
            mainMenu.setState(MenuState::MAIN_MENU);
          }
        }

        if (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::Enter) {
          std::cout << "[Menu] Enter -> Confirm\n";
          handleMenuSelection();
        }
      } else {
        if (event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::Enter) {
          std::cout << "[Menu] Enter -> Select\n";
          handleMenuSelection();
        }

        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left) {
          std::cout << "[Menu] Mouse click -> Select\n";
          handleMenuSelection();
        }
      }

      continue;
    }

    if (gameState == GameState::Playing) {
      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
          std::cout << "[GameEngine] Pause -> Menu\n";
          mainMenu.setState(MenuState::PAUSE);
          gameState = GameState::Menu;
        } else {
          handleKeyPress(event.key.code);
        }
      }
    }
  }
}

void GameEngine::update(float deltaTime) {
  fallTimer += deltaTime;

  if (fallTimer >= fallSpeed) {
    fallTimer = 0.0f;

    std::cout << "[GameEngine] Falling, y=" << currentTetromino.getY() << "\n";
    currentTetromino.moveDown();

    if (checkCollision()) {
      currentTetromino.moveUp();

      if (!isLocking) {
        std::cout << "[GameEngine] Start lock delay\n";
        isLocking = true;
        lockTimer = 0.0f;
      }
    } else {
      if (isLocking) {
        std::cout << "[GameEngine] Cancel lock delay\n";
        isLocking = false;
        lockTimer = 0.0f;
      }
    }
  }

  if (isLocking) {
    lockTimer += deltaTime;

    if (lockTimer >= lockDelay) {
      std::cout << "[GameEngine] Lock piece\n";

      lockTetromino();
      spawnNewTetromino();

      isLocking = false;
      lockTimer = 0.0f;
      fallTimer = 0.0f;
    }
  }
}

void GameEngine::render() {
  window.clear(sf::Color::White);

  if (gameState == GameState::Menu) {
    mainMenu.render(window);
  } else if (gameState == GameState::Playing) {
    board.render(window);
    currentTetromino.render(window);
  } else if (gameState == GameState::GameOver) {
    board.render(window);
    renderGameOver();
    mainMenu.render(window);
  }

  window.display();
}

void GameEngine::shutdown() { std::cout << "[GameEngine] Shutdown\n"; }

bool GameEngine::checkCollision() {
  const auto &shape = currentTetromino.getShape();
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
  const auto &shape = currentTetromino.getShape();
  int tetrominoX = currentTetromino.getX();
  int tetrominoY = currentTetromino.getY();

  board.lockTetromino(tetrominoX, tetrominoY, shape,
                      currentTetromino.getType());
  std::cout << "[GameEngine] Locked at (" << tetrominoX << ", " << tetrominoY
            << ")\n";

  int linesCleared = board.clearFullLines();
  if (linesCleared > 0) {
    std::cout << "[GameEngine] Cleared lines: " << linesCleared << "\n";
  }
}

void GameEngine::spawnNewTetromino() {
  std::cout << "[GameEngine] Spawn new tetromino\n";

  static std::mt19937 generator(static_cast<unsigned int>(std::time(nullptr)));
  std::uniform_int_distribution<int> distribution(0, 6);

  int randomType = distribution(generator);
  std::cout << "[GameEngine] Random type=" << randomType << "\n";

  isLocking = false;
  lockTimer = 0.0f;

  currentTetromino = Tetromino(static_cast<TetrominoType>(randomType));

  if (checkCollision()) {
    std::cout << "[GameEngine] GAME OVER - no space for new piece\n";
    gameState = GameState::GameOver;
  }
}

void GameEngine::handleMenuSelection() {
  MenuAction action = mainMenu.handleSelection();
  std::cout << "[GameEngine] Menu action=" << static_cast<int>(action) << "\n";

  switch (action) {
  case MenuAction::START_GAME:
    std::cout << "[Menu] -> Difficulty selection\n";
    mainMenu.setState(MenuState::DIFFICULTY_SELECTION);
    break;

  case MenuAction::CONFIRM_DIFFICULTY:
    std::cout << "[Menu] Start game\n";
    fallSpeed = mainMenu.getDifficultySpeed();
    board.reset();
    gameState = GameState::Playing;
    spawnNewTetromino();
    break;

  case MenuAction::RESUME:
    std::cout << "[Menu] Resume\n";
    gameState = GameState::Playing;
    break;

  case MenuAction::RESTART:
    std::cout << "[Menu] Restart\n";
    board.reset();
    mainMenu.setState(MenuState::DIFFICULTY_SELECTION);
    gameState = GameState::Menu;
    break;

  case MenuAction::MAIN_MENU:
    std::cout << "[Menu] Main menu\n";
    board.reset();
    mainMenu.setState(MenuState::MAIN_MENU);
    gameState = GameState::Menu;
    break;

  case MenuAction::EXIT:
    std::cout << "[Menu] Exit\n";
    window.close();
    break;

  default:
    break;
  }
}

void GameEngine::renderGameOver() {
  sf::RectangleShape overlay(sf::Vector2f(500, 700));
  overlay.setPosition(0, 0);
  overlay.setFillColor(sf::Color(0, 0, 0, 150));
  window.draw(overlay);
}

void GameEngine::handleKeyPress(sf::Keyboard::Key key) {
  if (key == sf::Keyboard::Left) {
    std::cout << "[Input] LEFT\n";
    currentTetromino.moveLeft();
    if (checkCollision()) {
      std::cout << "[Input] Collision -> undo LEFT\n";
      currentTetromino.moveRight();
    } else if (isLocking) {
      lockTimer = 0.0f;
    }
  } else if (key == sf::Keyboard::Right) {
    std::cout << "[Input] RIGHT\n";
    currentTetromino.moveRight();
    if (checkCollision()) {
      std::cout << "[Input] Collision -> undo RIGHT\n";
      currentTetromino.moveLeft();
    } else if (isLocking) {
      lockTimer = 0.0f;
    }
  } else if (key == sf::Keyboard::Down) {
    std::cout << "[Input] DOWN\n";
    currentTetromino.moveDown();
    if (checkCollision()) {
      currentTetromino.moveUp();
      if (!isLocking) {
        isLocking = true;
        lockTimer = 0.0f;
      }
    }
  } else if (key == sf::Keyboard::Space) {
    std::cout << "[Input] SPACE (hard drop)\n";
    while (!checkCollision()) {
      currentTetromino.moveDown();
    }
    currentTetromino.moveUp();
    lockTetromino();
    spawnNewTetromino();
  } else if (key == sf::Keyboard::Up) {
    std::cout << "[Input] Gracz naciska: GÓRA (rotacja + wall kick)\n";

    // Zapamiętujemy starą pozycję, żeby zawsze móc wrócić
    const int startX = currentTetromino.getX();
    const int startY = currentTetromino.getY();

    // Obracamy klocek (na chwilę) i sprawdzamy czy da się go "ustawić" kickami
    currentTetromino.rotate();

    // Lista prostych kicków (kolejność ma znaczenie!).
    // Najpierw próbujemy bez przesunięcia, potem o 1/2 w lewo/prawo,
    // potem lekko do góry (bo często obrót przy podłodze wymaga podbicia).
    const std::vector<std::pair<int, int>> kickTests = {
        {0, 0},  {-1, 0},  {1, 0},  {-2, 0}, {2, 0},
        {0, -1}, {-1, -1}, {1, -1}, {0, -2}};

    auto moveBy = [&](int dx, int dy) {
      // Uwaga: używamy Twoich metod moveLeft/moveRight/moveUp/moveDown
      // i robimy to pętlami, żeby nie zmieniać klasy Tetromino.
      if (dx > 0) {
        for (int i = 0; i < dx; i++)
          currentTetromino.moveRight();
      } else if (dx < 0) {
        for (int i = 0; i < -dx; i++)
          currentTetromino.moveLeft();
      }

      if (dy > 0) {
        for (int i = 0; i < dy; i++)
          currentTetromino.moveDown();
      } else if (dy < 0) {
        for (int i = 0; i < -dy; i++)
          currentTetromino.moveUp();
      }
    };

    auto resetToStartPos = [&]() {
      const int currentX = currentTetromino.getX();
      const int currentY = currentTetromino.getY();
      moveBy(startX - currentX, startY - currentY);
    };

    bool rotationPlaced = false;

    for (size_t i = 0; i < kickTests.size(); i++) {
      // Zawsze zaczynamy próbę od pozycji startowej
      resetToStartPos();

      const int kickX = kickTests[i].first;
      const int kickY = kickTests[i].second;

      // Próbujemy przesunięcie
      moveBy(kickX, kickY);

      // Jeśli NIE ma kolizji — obrót się udał
      if (!checkCollision()) {
        std::cout << "[WallKick] Rotacja OK, test=" << i << " kick=(" << kickX
                  << "," << kickY << ")\n";
        rotationPlaced = true;

        // Udany ruch powinien resetować lock delay (żeby dało się jeszcze
        // manewrować)
        if (isLocking) {
          lockTimer = 0.0f;
          std::cout << "[WallKick] Reset lockTimer po udanej rotacji\n";
        }
        break;
      } else {
        std::cout << "[WallKick] Test nieudany, test=" << i << " kick=("
                  << kickX << "," << kickY << ")\n";
      }
    }

    if (!rotationPlaced) {
      // Nic nie pasuje — wracamy do pozycji i cofamy obrót (3x rotate = -90
      // stopni)
      std::cout << "[WallKick] Rotacja zablokowana - cofanie\n";
      resetToStartPos();

      currentTetromino.rotate();
      currentTetromino.rotate();
      currentTetromino.rotate();
    }
  }
}
