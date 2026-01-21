#include "GameEngine.h"

#include <iostream>

GameEngine::GameEngine() : window(sf::VideoMode(500, 700), "Tetris") {}

void GameEngine::initialize() {
  isRunning = true;
  gameState = GameState::Menu;  // Zmieniamy na Menu zamiast Playing
  
  // Inicjalizacja generatora liczb losowych
  std::srand(static_cast<unsigned int>(std::time(nullptr)));
  
  // Inicjalizacja menu
  menu.initialize();
  menu.setState(MenuState::MAIN_MENU);
  
  std::cout << "Inicjalizacja gry - rozpoczęcie od menu" << '\n';
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
    
    // Obsługa zdarzeń w zależności od stanu gry
    if (gameState == GameState::Menu) {
      menu.handleEvent(event);
      
      // Dla ekranu wyboru trudności - sprawdzamy kliknięcia w przyciski
      if (menu.getState() == MenuState::DIFFICULTY_SELECTION) {
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
          int clickResult = menu.checkDifficultyClick(
            static_cast<float>(event.mouseButton.x),
            static_cast<float>(event.mouseButton.y)
          );
          
          if (clickResult == 1) {  // Confirm
            std::cout << "Kliknięto Confirm" << std::endl;
            handleMenuSelection();
          } else if (clickResult == 2) {  // Back
            std::cout << "Kliknięto Back" << std::endl;
            menu.setState(MenuState::MAIN_MENU);
          }
        }
        
        // Enter również potwierdza
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
          handleMenuSelection();
        }
      } 
      // Dla innych ekranów menu
      else {
        // Sprawdzamy czy naciśnięto Enter lub kliknięto myszką
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
          handleMenuSelection();
        }
        
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
          handleMenuSelection();
        }
      }
    } else if (gameState == GameState::Playing) {
      // Obsługa pauzy
      if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
        std::cout << "Pauza" << std::endl;
        menu.setState(MenuState::PAUSE);
        gameState = GameState::Menu;
      }
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
  
  if (gameState == GameState::Menu) {
    // Renderujemy tylko menu
    menu.render(window);
  } else if (gameState == GameState::Playing) {
    // Renderujemy grę
    board.render(window);
    currentTetromino.render(window);
  } else if (gameState == GameState::GameOver) {
    // Renderujemy planszę i menu Game Over
    board.render(window);
    menu.render(window);
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

/**
 * Obsługuje wybór opcji w menu.
 */
void GameEngine::handleMenuSelection() {
  MenuAction action = menu.handleSelection();
  
  std::cout << "GameEngine - obsługa akcji menu: " << static_cast<int>(action) << std::endl;
  
  switch (action) {
    case MenuAction::START_GAME:
      std::cout << "Przechodzenie do wyboru trudności" << std::endl;
      menu.setState(MenuState::DIFFICULTY_SELECTION);
      break;
      
    case MenuAction::CONFIRM_DIFFICULTY:
      std::cout << "Rozpoczynanie gry z poziomem: " << menu.getSelectedDifficulty() << std::endl;
      fallSpeed = menu.getDifficultySpeed();
      board.reset();  // Czyścimy planszę przed nową grą
      gameState = GameState::Playing;
      spawnNewTetromino();
      break;
      
    case MenuAction::HIGH_SCORES:
      std::cout << "Wyświetlanie high scores (TODO)" << std::endl;
      break;
      
    case MenuAction::SETTINGS:
      std::cout << "Otwieranie ustawień" << std::endl;
      menu.setState(MenuState::SETTINGS);
      break;
      
    case MenuAction::EXIT:
      std::cout << "Zamykanie gry" << std::endl;
      window.close();
      break;
      
    case MenuAction::RESUME:
      std::cout << "Wznawianie gry" << std::endl;
      gameState = GameState::Playing;
      break;
      
    case MenuAction::RESTART:
      std::cout << "Restart gry - powrót do wyboru trudności" << std::endl;
      board.reset();  // Czyścimy planszę przed restartem
      menu.setState(MenuState::DIFFICULTY_SELECTION);
      gameState = GameState::Menu;
      break;
      
    case MenuAction::MAIN_MENU:
      std::cout << "Powrót do menu głównego" << std::endl;
      board.reset();  // Czyścimy planszę przy powrocie do menu
      menu.setState(MenuState::MAIN_MENU);
      gameState = GameState::Menu;
      break;
      
    default:
      break;
  }
}

void GameEngine::renderGameOver() {
  // Półprzezroczyste czarne tło na całym oknie
  sf::RectangleShape overlay(sf::Vector2f(500, 700));
  overlay.setPosition(0, 0);
  overlay.setFillColor(sf::Color(0, 0, 0, 150));
  window.draw(overlay);
}


