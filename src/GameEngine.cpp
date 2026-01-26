#include "GameEngine.h"

#include <ctime>
#include <iostream>
#include <random>

GameEngine::GameEngine() : window(sf::VideoMode(720, 700), "Tetris") {}

void GameEngine::initialize() {
  isRunning = true;
  gameState = GameState::Menu;

  mainMenu.initialize();
  mainMenu.setState(MenuState::MAIN_MENU);
  
  score.initialize();
  score.reset();
  
  if (!uiFont.loadFromFile("../../resources/fonts/SourceSansPro-Regular.otf")) {
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
  fallTimer = 0.0f;
  lockTimer = 0.0f;
  isLocking = false;

  std::cout << "[GameEngine] Initialize -> Menu\n";
}

void GameEngine::run() {
  // Sprawdź czy kontroler jest podłączony
  if (sf::Joystick::isConnected(0)) {
    std::cout << "[GameEngine] Kontroler Xbox wykryty!\n";
  }

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

    //Obsługa kontrolera w menu
    if (event.type == sf::Event::JoystickButtonPressed) {
      if (gameState == GameState::Menu) {
        handleControllerButtonMenu(event.joystickButton.button);
      } else if (gameState == GameState::Playing) {
        handleControllerButton(event.joystickButton.button);
      }
    }
    
    if (event.type == sf::Event::JoystickMoved) {
      if (gameState == GameState::Menu) {
        handleControllerAxisMenu(event.joystickMove.axis, event.joystickMove.position);
      } else if (gameState == GameState::Playing) {
        handleControllerAxis(event.joystickMove.axis, event.joystickMove.position);
      }
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
    score.render(window);
    
    // DODANE: Renderuj Next i Hold
    renderNextPiece(window);
    renderHeldPiece(window);
    
  } else if (gameState == GameState::GameOver) {
    board.render(window);
    score.render(window);
    renderNextPiece(window);
    renderHeldPiece(window);
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
    score.addScore(linesCleared, score.getLevel());
  } else {
    score.resetCombo();
  }
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
  
  // Pobierz pierwszy klocek z kolejki
  currentTetromino = Tetromino(nextQueue[0]);
  std::cout << "[GameEngine] Spawned type=" << static_cast<int>(nextQueue[0]) << "\n";
  
  // Przesuń kolejkę i dodaj nowy na końcu
  nextQueue.erase(nextQueue.begin());
  nextQueue.push_back(getRandomTetrominoType());
  
  canHold = true;

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
    score.reset();
    nextQueue.clear();
    for (int i = 0; i < 5; i++) {
      nextQueue.push_back(getRandomTetrominoType());
    }
    heldTetrominoType = TetrominoType::Empty;
    hasHeldPiece = false;
    canHold = true;
    for (int i = 0; i < mainMenu.getSelectedDifficulty(); i++) {
      score.increaseLevel();
    }
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
    score.reset();
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
  if (key == sf::Keyboard::H) {
    std::cout << "[Input] H (hold)\n";
    holdCurrentPiece();
    return;
  }
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
/**
 * Pomocnicza funkcja do renderowania podglądu klocka.
 * NAPRAWIONE: Nie tworzy nowego Tetromino, tylko bezpośrednio rysuje kształt.
 */
void GameEngine::renderTetrominoPreview(sf::RenderWindow& window, TetrominoType type, float x, float y) const {
  if (type == TetrominoType::Empty) {
    return;
  }
  
  const int CELL_SIZE = 20;
  
  // Bezpośrednie mapowanie typu na kolor i kształt (BEZ tworzenia Tetromino)
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
 * Renderuje panel "NEXT" z następnym klockiem.
 * Pozycja: x=540, y=160 (piąta kratka od góry = 50 + 5*30 = 200, ale -40 dla wyrównania)
 */
void GameEngine::renderNextPiece(sf::RenderWindow& window) const {
  // ZMIENIONE: Wyższy panel dla 5 klocków
  sf::RectangleShape panel(sf::Vector2f(150, 450));
  panel.setPosition(590, 160);
  panel.setFillColor(sf::Color(50, 50, 50));
  panel.setOutlineThickness(2);
  panel.setOutlineColor(sf::Color::White);
  window.draw(panel);
  
  sf::Text titleText;
  titleText.setFont(uiFont);
  titleText.setString("NEXT");
  titleText.setCharacterSize(20);
  titleText.setFillColor(sf::Color::Green);
  titleText.setPosition(610, 170);
  window.draw(titleText);
  
  // Renderuj 5 następnych klocków
  for (size_t i = 0; i < nextQueue.size() && i < 5; i++) {
    float yPos = 210 + i * 85;  // 85px odstępu między klockami
    renderTetrominoPreview(window, nextQueue[i], 620, yPos);
  }
}

/**
 * Renderuje panel "HOLD" z zatrzymanym klockiem.
 * Pozycja: x=60, y=160 (tak samo jak Next, ale po lewej)
 */
void GameEngine::renderHeldPiece(sf::RenderWindow& window) const {
  // ZMIENIONE: Większy panel (140 wysokości)
  sf::RectangleShape panel(sf::Vector2f(150, 140));
  panel.setPosition(60, 160);
  panel.setFillColor(sf::Color(50, 50, 50));
  panel.setOutlineThickness(2);
  panel.setOutlineColor(sf::Color::White);
  window.draw(panel);
  
  sf::Text titleText;
  titleText.setFont(uiFont);
  titleText.setString("HOLD");
  titleText.setCharacterSize(20);
  titleText.setFillColor(sf::Color::Magenta);
  titleText.setPosition(80, 170);
  window.draw(titleText);
  
  if (hasHeldPiece) {
    renderTetrominoPreview(window, heldTetrominoType, 90, 210);
  }
  
  if (!canHold) {
    sf::RectangleShape overlay(sf::Vector2f(150, 140));  // ZMIENIONE
    overlay.setPosition(60, 160);
    overlay.setFillColor(sf::Color(0, 0, 0, 100));
    window.draw(overlay);
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
  
  const float threshold = 50.0f;  // Próg aktywacji
  
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
  
  // D-pad góra/dół (Axis::PovY)
  if (axis == sf::Joystick::PovY) {
    if (position < -threshold && !dpadDownPressed) {
      std::cout << "[Controller] D-pad DOWN\n";
      handleKeyPress(sf::Keyboard::Down);
      dpadDownPressed = true;
    } else if (position > -threshold) {
      dpadDownPressed = false;
    }
  }
}

/**
 * Obsługa przycisków kontrolera w menu.
 */
void GameEngine::handleControllerButtonMenu(unsigned int button) {
  std::cout << "[Controller Menu] Button: " << button << "\n";
  
  switch (button) {
    case 0:  // A - Potwierdź
      std::cout << "[Controller Menu] A (confirm)\n";
      handleMenuSelection();
      break;
      
    case 1:  // B - Back
      std::cout << "[Controller Menu] B (back)\n";
      if (mainMenu.getState() == MenuState::DIFFICULTY_SELECTION) {
        mainMenu.setState(MenuState::MAIN_MENU);
      } else if (mainMenu.getState() == MenuState::PAUSE) {
        // B w pauzie - wznów grę
        gameState = GameState::Playing;
      }
      break;
      
    case 7:  // Start/Menu
      std::cout << "[Controller Menu] Start\n";
      if (mainMenu.getState() == MenuState::PAUSE) {
        // Menu w pauzie - wznów grę
        gameState = GameState::Playing;
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
  
  // D-pad góra/dół (PovY) lub lewy joystick (Y)
  if (axis == sf::Joystick::PovY || axis == sf::Joystick::Y) {
    if (position < -threshold && !upPressed) {
      std::cout << "[Controller Menu] UP\n";
      
      // W menu difficulty selection: nawigacja między elementami
      if (mainMenu.getState() == MenuState::DIFFICULTY_SELECTION) {
        sf::Event fakeEvent;
        fakeEvent.type = sf::Event::KeyPressed;
        fakeEvent.key.code = sf::Keyboard::Up;
        mainMenu.handleEvent(fakeEvent);
      } else {
        mainMenu.moveUp();
      }
      upPressed = true;
    } else if (position > threshold && !downPressed) {
      std::cout << "[Controller Menu] DOWN\n";
      
      if (mainMenu.getState() == MenuState::DIFFICULTY_SELECTION) {
        sf::Event fakeEvent;
        fakeEvent.type = sf::Event::KeyPressed;
        fakeEvent.key.code = sf::Keyboard::Down;
        mainMenu.handleEvent(fakeEvent);
      } else {
        mainMenu.moveDown();
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