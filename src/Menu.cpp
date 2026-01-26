#include "Menu.h"

#include <iostream>

Menu::Menu()
    : currentState(MenuState::MAIN_MENU), selectedIndex(0),
      selectedDifficulty(0), selectedDifficultyElement(2) {
  std::cout << "Menu - konstruktor" << std::endl;
}

Menu::~Menu() { std::cout << "Menu - destruktor" << std::endl; }

/**
 * Inicjalizacja menu - ładuje czcionkę i tworzy teksty.
 */
void Menu::initialize() {
  std::cout << "Menu - inicjalizacja" << std::endl;

  // Ładowanie czcionki z głównego katalogu projektu
  // Kiedy uruchamiasz z build/Debug/, ścieżka "../.." cofa się do głównego
  // folderu, możliwa potrzeba modyfikacji później przy zmianie miejsca pliku
  // .exe
  if (!font.loadFromFile("../../resources/fonts/SourceSansPro-Regular.otf")) {
    std::cout << "[ERROR] Nie udało się załadować czcionki!" << std::endl;
    std::cout << "[INFO] Sprawdź czy plik istnieje w: "
                 "../resources/fonts/SourceSansPro-Regular.otf"
              << std::endl;
    std::cout << "[INFO] (relatywnie do build/Debug/)" << std::endl;
    return;
  }

  std::cout << "Czcionka załadowana pomyślnie" << std::endl;

  // Konfiguracja tytułu
  titleText.setFont(font);
  titleText.setCharacterSize(TITLE_SIZE);
  titleText.setFillColor(sf::Color::White);

  // Konfiguracja tekstów dla wyboru trudności
  difficultyText.setFont(font);
  difficultyText.setCharacterSize(40);
  difficultyText.setFillColor(sf::Color::White);

  leftArrowText.setFont(font);
  leftArrowText.setString("<");
  leftArrowText.setCharacterSize(50);
  leftArrowText.setFillColor(sf::Color::White);

  rightArrowText.setFont(font);
  rightArrowText.setString(">");
  rightArrowText.setCharacterSize(50);
  rightArrowText.setFillColor(sf::Color::White);

  // Przycisk Confirm - WAŻNE: najpierw font, potem string!
  confirmButton.setFont(font);
  confirmButton.setString("Confirm");
  confirmButton.setCharacterSize(30);
  confirmButton.setFillColor(sf::Color::White);

  // Przycisk Back - WAŻNE: najpierw font, potem string!
  backButton.setFont(font);
  backButton.setString("Back");
  backButton.setCharacterSize(30);
  backButton.setFillColor(sf::Color::White);

  // DEBUG - sprawdzamy czy teksty mają zawartość
  std::cout << "[DEBUG] Confirm text: '"
            << confirmButton.getString().toAnsiString() << "'" << std::endl;
  std::cout << "[DEBUG] Back text: '" << backButton.getString().toAnsiString()
            << "'" << std::endl;

  // Ustawienie początkowego stanu menu
  setState(MenuState::MAIN_MENU);
}

/**
 * Ustawia stan menu i aktualizuje listę opcji.
 */
void Menu::setState(MenuState state) {
  std::cout << "Menu - zmiana stanu na: " << static_cast<int>(state)
            << std::endl;

  currentState = state;
  selectedIndex = 0;
  menuItems.clear();
  itemTexts.clear();

  // Konfiguracja tytułu i opcji w zależności od stanu
  switch (currentState) {
  case MenuState::MAIN_MENU:
    titleText.setString("TETRIS"); // Nazwa do dyskusji
    menuItems = {"Play", "High Scores", "Settings", "Quit"};
    break;

  case MenuState::PAUSE:
    titleText.setString("PAUSED");
    menuItems = {"Resume", "Restart", "Main Menu"};
    break;

  case MenuState::GAME_OVER:
    titleText.setString("GAME OVER");
    menuItems = {"Restart", "Main Menu", "Quit"};
    break;

  case MenuState::SETTINGS:
    titleText.setString("SETTINGS");
    menuItems = {"Back"};
    break;

  case MenuState::DIFFICULTY_SELECTION:
    titleText.setString("SELECT DIFFICULTY");
    selectedDifficultyElement = 2; // Startujemy na Confirm
    // Nie używamy standardowych menuItems dla tego ekranu
    updateDifficultyDisplay();
    break;

  default:
    break;
  }

  // Wycentrowanie tytułu
  sf::FloatRect titleBounds = titleText.getLocalBounds();
  titleText.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
  titleText.setPosition(WINDOW_WIDTH / 2.0f, 150.0f);

  // Tworzenie obiektów tekstowych dla każdej opcji menu
  if (currentState != MenuState::DIFFICULTY_SELECTION) {
    int startY = 300;

    for (size_t i = 0; i < menuItems.size(); i++) {
      sf::Text itemText;
      itemText.setFont(font);
      itemText.setString(menuItems[i]);
      itemText.setCharacterSize(ITEM_SIZE);
      itemText.setFillColor(sf::Color::White);

      sf::FloatRect bounds = itemText.getLocalBounds();
      itemText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
      itemText.setPosition(WINDOW_WIDTH / 2.0f, startY + i * ITEM_SPACING);

      itemTexts.push_back(itemText);
    }
  }

  std::cout << "Menu ma " << menuItems.size() << " opcji" << std::endl;
}

/**
 * Aktualizuje wyświetlany tekst poziomu trudności.
 */
void Menu::updateDifficultyDisplay() {
  // Aktualizacja tekstu "Level: X"
  difficultyText.setString("Level: " + std::to_string(selectedDifficulty));

  // Wycentrowanie tekstu głównego
  sf::FloatRect diffBounds = difficultyText.getLocalBounds();
  difficultyText.setOrigin(diffBounds.width / 2.0f, diffBounds.height / 2.0f);
  difficultyText.setPosition(WINDOW_WIDTH / 2.0f, 350.0f);

  // Pozycjonowanie lewej strzałki
  sf::FloatRect leftBounds = leftArrowText.getLocalBounds();
  leftArrowText.setOrigin(leftBounds.width / 2.0f, leftBounds.height / 2.0f);
  leftArrowText.setPosition(WINDOW_WIDTH / 2.0f - 150.0f, 350.0f);

  // Pozycjonowanie prawej strzałki
  sf::FloatRect rightBounds = rightArrowText.getLocalBounds();
  rightArrowText.setOrigin(rightBounds.width / 2.0f, rightBounds.height / 2.0f);
  rightArrowText.setPosition(WINDOW_WIDTH / 2.0f + 150.0f, 350.0f);

  // ZAMIANA MIEJSC: Back po lewej, Confirm po prawej
  // Pozycjonowanie przycisku Back (po lewej stronie)
  sf::FloatRect backBounds = backButton.getLocalBounds();
  backButton.setOrigin(backBounds.width / 2.0f, backBounds.height / 2.0f);
  backButton.setPosition(WINDOW_WIDTH / 2.0f - 80.0f, 500.0f);

  // Pozycjonowanie przycisku Confirm (po prawej stronie)
  sf::FloatRect confirmBounds = confirmButton.getLocalBounds();
  confirmButton.setOrigin(confirmBounds.width / 2.0f,
                          confirmBounds.height / 2.0f);
  confirmButton.setPosition(WINDOW_WIDTH / 2.0f + 80.0f, 500.0f);

  std::cout << "Zaktualizowano wyświetlanie trudności: Level "
            << selectedDifficulty << std::endl;
}

/**
 * Zwiększa wybrany poziom trudności (z zawijaniem).
 */
void Menu::increaseDifficulty() {
  selectedDifficulty++;
  if (selectedDifficulty > MAX_DIFFICULTY) {
    selectedDifficulty = MIN_DIFFICULTY; // Zawijanie 9 -> 0
  }
  updateDifficultyDisplay();
  std::cout << "Zwiększono trudność do: " << selectedDifficulty << std::endl;
}

/**
 * Zmniejsza wybrany poziom trudności (z zawijaniem).
 */
void Menu::decreaseDifficulty() {
  selectedDifficulty--;
  if (selectedDifficulty < MIN_DIFFICULTY) {
    selectedDifficulty = MAX_DIFFICULTY; // Zawijanie 0 -> 9
  }
  updateDifficultyDisplay();
  std::cout << "Zmniejszono trudność do: " << selectedDifficulty << std::endl;
}

/**
 * Konwertuje poziom trudności na prędkość spadania klocków.
 * Używa formuły z oryginalnego NES Tetris:
 * Level 0: 48 klatek (0.8s przy 60 FPS)
 * Level 1-8: stopniowo przyspiesza
 * Level 9: 6 klatek (0.1s przy 60 FPS)
 * Level 29+: 1 klatka (killscreen)
 *
 * Zwracamy czas w sekundach na podstawie liczby klatek przy 60 FPS.
 */
float Menu::getDifficultySpeed() const {
  // Tablica klatek dla każdego poziomu (0-9) z NES Tetris
  const int framesPerDrop[] = {
      48, // Level 0
      43, // Level 1
      38, // Level 2
      33, // Level 3
      28, // Level 4
      23, // Level 5
      18, // Level 6
      13, // Level 7
      8,  // Level 8
      6   // Level 9
  };

  if (selectedDifficulty >= 0 && selectedDifficulty <= 9) {
    // Konwersja klatek na sekundy (60 FPS = 1/60 sekundy na klatkę)
    return framesPerDrop[selectedDifficulty] / 60.0f;
  }

  return 0.8f; // Domyślna wartość (Level 0)
}

/**
 * Przesuwa wybór w górę (z zawijaniem).
 */
void Menu::moveUp() {
  selectedIndex--;
  if (selectedIndex < 0) {
    selectedIndex = menuItems.size() - 1;
  }
  std::cout << "Menu - wybrano opcję: " << selectedIndex << " ("
            << menuItems[selectedIndex] << ")" << std::endl;
}

/**
 * Przesuwa wybór w dół (z zawijaniem).
 */
void Menu::moveDown() {
  selectedIndex++;
  if (selectedIndex >= static_cast<int>(menuItems.size())) {
    selectedIndex = 0;
  }
  std::cout << "Menu - wybrano opcję: " << selectedIndex << " ("
            << menuItems[selectedIndex] << ")" << std::endl;
}

/**
 * Zwraca akcję odpowiadającą wybranej opcji menu.
 */
MenuAction Menu::handleSelection() {
  // Dla ekranu wyboru trudności sprawdzamy który element jest wybrany
  if (currentState == MenuState::DIFFICULTY_SELECTION) {
    if (selectedDifficultyElement == 2) { // Confirm
      std::cout << "Menu - potwierdzono poziom trudności: "
                << selectedDifficulty << std::endl;
      return MenuAction::CONFIRM_DIFFICULTY;
    } else if (selectedDifficultyElement == 1) { // Back
      std::cout << "Menu - powrót do menu głównego z wyboru trudności"
                << std::endl;
      return MenuAction::MAIN_MENU;
    }
    // Dla selektora poziomu Enter przenosi na Confirm
    else if (selectedDifficultyElement == 0) {
      selectedDifficultyElement = 2;
      std::cout << "[DEBUG] Enter na selektorze - przejście do Confirm"
                << std::endl;
      return MenuAction::NONE;
    }
  }

  // Dla innych ekranów
  if (!menuItems.empty()) {
    std::cout << "Menu - potwierdzono wybór: " << menuItems[selectedIndex]
              << std::endl;
  }

  // Mapowanie opcji na akcje w zależności od stanu menu
  switch (currentState) {
  case MenuState::MAIN_MENU:
    switch (selectedIndex) {
    case 0:
      return MenuAction::START_GAME;
    case 1:
      return MenuAction::HIGH_SCORES;
    case 2:
      return MenuAction::SETTINGS;
    case 3:
      return MenuAction::EXIT;
    }
    break;

  case MenuState::PAUSE:
    switch (selectedIndex) {
    case 0:
      return MenuAction::RESUME;
    case 1:
      return MenuAction::RESTART;
    case 2:
      return MenuAction::MAIN_MENU;
    }
    break;

  case MenuState::GAME_OVER:
    switch (selectedIndex) {
    case 0:
      return MenuAction::RESTART;
    case 1:
      return MenuAction::MAIN_MENU;
    case 2:
      return MenuAction::EXIT;
    }
    break;

  case MenuState::SETTINGS:
    if (selectedIndex == 0)
      return MenuAction::MAIN_MENU;
    break;

  default:
    break;
  }

  return MenuAction::NONE;
}

/**
 * Obsługuje zdarzenia klawiatury i myszy.
 */
void Menu::handleEvent(const sf::Event &event) {
  // Specjalna obsługa dla ekranu wyboru trudności
  if (currentState == MenuState::DIFFICULTY_SELECTION) {
    if (event.type == sf::Event::KeyPressed) {
      // Nawigacja w zależności od wybranego elementu
      if (selectedDifficultyElement == 0) {
        // Jesteśmy na selektorze poziomu (strzałki + Level: X)
        if (event.key.code == sf::Keyboard::Left) {
          decreaseDifficulty();
        } else if (event.key.code == sf::Keyboard::Right) {
          increaseDifficulty();
        } else if (event.key.code == sf::Keyboard::Down) {
          // W dół: przejdź do Confirm
          selectedDifficultyElement = 2;
          std::cout << "[DEBUG] Wybrano element: Confirm" << std::endl;
        } else if (event.key.code == sf::Keyboard::Enter) {
          // Enter na selektorze: przejdź do Confirm
          selectedDifficultyElement = 2;
          std::cout << "[DEBUG] Enter - przejście do Confirm" << std::endl;
        }
      } else if (selectedDifficultyElement == 1) {
        // Jesteśmy na Back
        if (event.key.code == sf::Keyboard::Left ||
            event.key.code == sf::Keyboard::Right) {
          // Lewo/Prawo: przełącz między Back a Confirm
          selectedDifficultyElement = 2; // Przejdź do Confirm
          std::cout << "[DEBUG] Przełączono na Confirm" << std::endl;
        } else if (event.key.code == sf::Keyboard::Up) {
          // Góra: przejdź do selektora poziomu
          selectedDifficultyElement = 0;
          std::cout << "[DEBUG] Wybrano element: Level Selector" << std::endl;
        }
      } else if (selectedDifficultyElement == 2) {
        // Jesteśmy na Confirm
        if (event.key.code == sf::Keyboard::Left ||
            event.key.code == sf::Keyboard::Right) {
          // Lewo/Prawo: przełącz między Confirm a Back
          selectedDifficultyElement = 1; // Przejdź do Back
          std::cout << "[DEBUG] Przełączono na Back" << std::endl;
        } else if (event.key.code == sf::Keyboard::Up) {
          // Góra: przejdź do selektora poziomu
          selectedDifficultyElement = 0;
          std::cout << "[DEBUG] Wybrano element: Level Selector" << std::endl;
        }
      }
    }

    // Obsługa kliknięć myszy
    if (event.type == sf::Event::MouseButtonPressed) {
      if (event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x),
                              static_cast<float>(event.mouseButton.y));

        // Kliknięcie w lewą strzałkę - zmniejsz poziom
        if (leftArrowText.getGlobalBounds().contains(mousePos)) {
          decreaseDifficulty();
          selectedDifficultyElement = 0; // Zaznacz selektor
        }
        // Kliknięcie w prawą strzałkę - zwiększ poziom
        else if (rightArrowText.getGlobalBounds().contains(mousePos)) {
          increaseDifficulty();
          selectedDifficultyElement = 0; // Zaznacz selektor
        }
        // Kliknięcie w tekst "Level: X" - zaznacz selektor
        else if (difficultyText.getGlobalBounds().contains(mousePos)) {
          selectedDifficultyElement = 0;
        }
      }
    }

    // Obsługa myszy - najechanie kursorem (aktualizacja
    // selectedDifficultyElement)
    if (event.type == sf::Event::MouseMoved) {
      sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x),
                            static_cast<float>(event.mouseMove.y));

      // Sprawdzamy nad którym elementem jest kursor
      if (confirmButton.getGlobalBounds().contains(mousePos)) {
        selectedDifficultyElement = 2;
      } else if (backButton.getGlobalBounds().contains(mousePos)) {
        selectedDifficultyElement = 1;
      } else if (leftArrowText.getGlobalBounds().contains(mousePos) ||
                 rightArrowText.getGlobalBounds().contains(mousePos) ||
                 difficultyText.getGlobalBounds().contains(mousePos)) {
        selectedDifficultyElement = 0;
      }
    }

    return; // Nie obsługujemy standardowej nawigacji dla tego ekranu
  }

  // Standardowa obsługa dla innych ekranów menu
  if (event.type == sf::Event::KeyPressed) {
    if (event.key.code == sf::Keyboard::Up) {
      moveUp();
    } else if (event.key.code == sf::Keyboard::Down) {
      moveDown();
    } else if (event.key.code == sf::Keyboard::Enter ||
               event.key.code == sf::Keyboard::Space) {
      // Wybór opcji zostanie obsłużony w GameEngine
    }
  }

  // Obsługa myszy - najechanie kursorem
  if (event.type == sf::Event::MouseMoved) {
    sf::Vector2f mousePos(static_cast<float>(event.mouseMove.x),
                          static_cast<float>(event.mouseMove.y));

    for (size_t i = 0; i < itemTexts.size(); i++) {
      if (itemTexts[i].getGlobalBounds().contains(mousePos)) {
        selectedIndex = i;
        break;
      }
    }
  }

  // Obsługa myszy - kliknięcie
  if (event.type == sf::Event::MouseButtonPressed) {
    if (event.mouseButton.button == sf::Mouse::Left) {
      sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x),
                            static_cast<float>(event.mouseButton.y));

      for (size_t i = 0; i < itemTexts.size(); i++) {
        if (itemTexts[i].getGlobalBounds().contains(mousePos)) {
          selectedIndex = i;
          break;
        }
      }
    }
  }
}

/**
 * Sprawdza czy kliknięto w przyciski Confirm lub Back.
 * Zwraca: 0 = nie kliknięto, 1 = Confirm, 2 = Back
 */
int Menu::checkDifficultyClick(float mouseX, float mouseY) const {
  sf::Vector2f mousePos(mouseX, mouseY);

  std::cout << "[DEBUG] Sprawdzanie kliknięcia w pozycji: (" << mouseX << ", "
            << mouseY << ")" << std::endl;

  // UWAGA: Zamieniliśmy miejsca - Confirm jest teraz po prawej
  if (confirmButton.getGlobalBounds().contains(mousePos)) {
    std::cout << "[DEBUG] Kliknięto w Confirm!" << std::endl;
    return 1; // Confirm
  }

  if (backButton.getGlobalBounds().contains(mousePos)) {
    std::cout << "[DEBUG] Kliknięto w Back!" << std::endl;
    return 2; // Back
  }

  std::cout << "[DEBUG] Nie kliknięto w żaden przycisk" << std::endl;
  return 0; // Nic nie kliknięto
}

/**
 * Renderuje menu na ekranie.
 */
void Menu::render(sf::RenderWindow &window) const {
  // Rysujemy półprzezroczyste tło
  sf::RectangleShape background(sf::Vector2f(720, 700));
  background.setFillColor(sf::Color(0, 0, 0, 200));
  window.draw(background);

  // Rysujemy tytuł
  window.draw(titleText);

  // Specjalne renderowanie dla ekranu wyboru trudności
  if (currentState == MenuState::DIFFICULTY_SELECTION) {
    // Kopiujemy elementy żeby móc je podświetlić
    sf::Text leftCopy = leftArrowText;
    sf::Text rightCopy = rightArrowText;
    sf::Text diffCopy = difficultyText;
    sf::Text confirmCopy = confirmButton;
    sf::Text backCopy = backButton;

    // Podświetlamy wybrany element
    if (selectedDifficultyElement == 0) {
      // Cały klocek selektora (strzałki + tekst) podświetlony
      leftCopy.setFillColor(sf::Color::Yellow);
      diffCopy.setFillColor(sf::Color::Yellow);
      rightCopy.setFillColor(sf::Color::Yellow);

      // Opcjonalnie: lekko zwiększ rozmiar
      leftCopy.setCharacterSize(55);
      diffCopy.setCharacterSize(45);
      rightCopy.setCharacterSize(55);

      // Ponowne wycentrowanie
      sf::FloatRect lb = leftCopy.getLocalBounds();
      leftCopy.setOrigin(lb.width / 2.0f, lb.height / 2.0f);
      leftCopy.setPosition(leftArrowText.getPosition());

      sf::FloatRect db = diffCopy.getLocalBounds();
      diffCopy.setOrigin(db.width / 2.0f, db.height / 2.0f);
      diffCopy.setPosition(difficultyText.getPosition());

      sf::FloatRect rb = rightCopy.getLocalBounds();
      rightCopy.setOrigin(rb.width / 2.0f, rb.height / 2.0f);
      rightCopy.setPosition(rightArrowText.getPosition());
    } else if (selectedDifficultyElement == 1) { // Back
      backCopy.setFillColor(sf::Color::Yellow);
      backCopy.setCharacterSize(35);
      sf::FloatRect bounds = backCopy.getLocalBounds();
      backCopy.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
      backCopy.setPosition(backButton.getPosition());
    } else if (selectedDifficultyElement == 2) { // Confirm
      confirmCopy.setFillColor(sf::Color::Yellow);
      confirmCopy.setCharacterSize(35);
      sf::FloatRect bounds = confirmCopy.getLocalBounds();
      confirmCopy.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
      confirmCopy.setPosition(confirmButton.getPosition());
    }

    // Rysujemy wszystkie elementy
    window.draw(leftCopy);
    window.draw(diffCopy);
    window.draw(rightCopy);
    window.draw(backCopy);
    window.draw(confirmCopy);
    return;
  }

  // Standardowe renderowanie opcji menu
  for (size_t i = 0; i < itemTexts.size(); i++) {
    sf::Text item = itemTexts[i];

    if (static_cast<int>(i) == selectedIndex) {
      item.setFillColor(sf::Color::Yellow);
      item.setCharacterSize(ITEM_SIZE + 5);

      sf::FloatRect bounds = item.getLocalBounds();
      item.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    }

    window.draw(item);
  }
}

/**
 * Wyświetla ekran Game Over z wynikiem.
 */
void Menu::showGameOver(int score, bool isHighScore) {
  std::cout << "Menu - Game Over, wynik: " << score << std::endl;

  setState(MenuState::GAME_OVER);

  // TODO: Tutaj możesz dodać wyświetlanie wyniku
  // Na razie tylko zmieniamy stan na GAME_OVER
  if (isHighScore) {
    std::cout << "Nowy rekord!" << std::endl;
  }
}
