#include "Menu.h"
#include <algorithm>
#include <iostream>

Menu::Menu()
    : currentState(MenuState::MAIN_MENU), selectedIndex(0),
      selectedDifficulty(1), selectedDifficultyElement(0) {}

Menu::~Menu() {}

void Menu::initialize() {
  if (!font.loadFromFile(
          "../resources/fonts/Press_Start_2P/PressStart2P-Regular.ttf")) {
    std::cout << "[Menu ERROR] Nie można załadować czcionki\n";
  }

  titleText.setFont(font);
  titleText.setString("TETRIS");
  titleText.setCharacterSize(TITLE_SIZE);
  titleText.setFillColor(sf::Color::Yellow);
  sf::FloatRect titleBounds = titleText.getLocalBounds();
  titleText.setOrigin(titleBounds.width / 2.0f, titleBounds.height / 2.0f);
  titleText.setPosition(WINDOW_WIDTH / 2.0f, 100);

  setState(MenuState::MAIN_MENU);

  // Inicjalizacja ekranu trudności
  difficultyText.setFont(font);
  difficultyText.setCharacterSize(30);
  difficultyText.setFillColor(sf::Color::White);
  updateDifficultyDisplay();

  leftArrowText.setFont(font);
  leftArrowText.setString("<");
  leftArrowText.setCharacterSize(40);
  leftArrowText.setFillColor(sf::Color::White);
  sf::FloatRect laBounds = leftArrowText.getLocalBounds();
  leftArrowText.setOrigin(laBounds.width / 2.0f, laBounds.height / 2.0f);
  leftArrowText.setPosition(250, 400);

  rightArrowText.setFont(font);
  rightArrowText.setString(">");
  rightArrowText.setCharacterSize(40);
  rightArrowText.setFillColor(sf::Color::White);
  sf::FloatRect raBounds = rightArrowText.getLocalBounds();
  rightArrowText.setOrigin(raBounds.width / 2.0f, raBounds.height / 2.0f);
  rightArrowText.setPosition(550, 400);

  // Przyciski na dole ekranu trudności
  backButton.setFont(font);
  backButton.setString("BACK");
  backButton.setCharacterSize(25);
  backButton.setFillColor(sf::Color::White);
  sf::FloatRect bBounds = backButton.getLocalBounds();
  backButton.setOrigin(bBounds.width / 2.0f, bBounds.height / 2.0f);
  backButton.setPosition(300, 600);

  confirmButton.setFont(font);
  confirmButton.setString("START");
  confirmButton.setCharacterSize(25);
  confirmButton.setFillColor(sf::Color::White);
  sf::FloatRect cBounds = confirmButton.getLocalBounds();
  confirmButton.setOrigin(cBounds.width / 2.0f, cBounds.height / 2.0f);
  confirmButton.setPosition(500, 600);
}

void Menu::setState(MenuState state) {
  currentState = state;
  selectedIndex = 0;
  menuItems.clear();
  itemTexts.clear();

  switch (currentState) {
  case MenuState::MAIN_MENU:
    menuItems = {"START GAME", "HIGH SCORES", "SETTINGS", "EXIT"};
    break;
  case MenuState::PAUSE:
    menuItems = {"RESUME", "RESTART", "MAIN MENU", "EXIT"};
    break;
  case MenuState::SETTINGS:
    // Dodajemy dynamicznie nazwę motywu
    std::string themeName = "CLASSIC";
    if (selectedTheme == 1)
      themeName = "DARK";
    if (selectedTheme == 2)
      themeName = "NEON";
    menuItems = {"THEME: " + themeName, "BACK"};
    break;
  }

  for (size_t i = 0; i < menuItems.size(); i++) {
    sf::Text text;
    text.setFont(font);
    text.setString(menuItems[i]);
    text.setCharacterSize(ITEM_SIZE);
    text.setFillColor(sf::Color::White);

    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    text.setPosition(WINDOW_WIDTH / 2.0f, 300 + i * ITEM_SPACING);

    itemTexts.push_back(text);
  }
}

void Menu::moveUp() {
  if (currentState == MenuState::DIFFICULTY_SELECTION) {
    selectedDifficultyElement--;
    if (selectedDifficultyElement < 0)
      selectedDifficultyElement = 2; // Selector -> Back -> Confirm
    return;
  }

  selectedIndex--;
  if (selectedIndex < 0) {
    selectedIndex = (int)menuItems.size() - 1;
  }
}

void Menu::moveDown() {
  if (currentState == MenuState::DIFFICULTY_SELECTION) {
    selectedDifficultyElement++;
    if (selectedDifficultyElement > 2)
      selectedDifficultyElement = 0;
    return;
  }

  selectedIndex++;
  if (selectedIndex >= (int)menuItems.size()) {
    selectedIndex = 0;
  }
}

void Menu::increaseDifficulty() {
  selectedDifficulty++;
  if (selectedDifficulty > MAX_DIFFICULTY)
    selectedDifficulty = MIN_DIFFICULTY;
  updateDifficultyDisplay();
}

void Menu::decreaseDifficulty() {
  selectedDifficulty--;
  if (selectedDifficulty < MIN_DIFFICULTY)
    selectedDifficulty = MAX_DIFFICULTY;
  updateDifficultyDisplay();
}

void Menu::updateDifficultyDisplay() {
  difficultyText.setString("LEVEL " + std::to_string(selectedDifficulty));
  sf::FloatRect bounds = difficultyText.getLocalBounds();
  difficultyText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
  difficultyText.setPosition(WINDOW_WIDTH / 2.0f, 400);
}

float Menu::getDifficultySpeed() const {
  static const float speeds[] = {0.8f, 0.7f,  0.6f, 0.5f,  0.4f,
                                 0.3f, 0.25f, 0.2f, 0.15f, 0.1f};
  if (selectedDifficulty >= 0 && selectedDifficulty <= 9) {
    return speeds[selectedDifficulty];
  }
  return 0.8f;
}

MenuAction Menu::handleSelection() {
  if (currentState == MenuState::DIFFICULTY_SELECTION) {
    if (selectedDifficultyElement == 0)
      return MenuAction::NONE;
    if (selectedDifficultyElement == 1)
      return MenuAction::MAIN_MENU;
    if (selectedDifficultyElement == 2)
      return MenuAction::CONFIRM_DIFFICULTY;
  }

  switch (currentState) {
  case MenuState::MAIN_MENU:
    if (selectedIndex == 0)
      return MenuAction::START_GAME; // To selection
    if (selectedIndex == 1)
      return MenuAction::HIGH_SCORES;
    if (selectedIndex == 2)
      return MenuAction::SETTINGS;
    if (selectedIndex == 3)
      return MenuAction::EXIT;
    break;
  case MenuState::PAUSE:
    if (selectedIndex == 0)
      return MenuAction::RESUME;
    if (selectedIndex == 1)
      return MenuAction::RESTART;
    if (selectedIndex == 2)
      return MenuAction::MAIN_MENU;
    if (selectedIndex == 3)
      return MenuAction::EXIT;
    break;
  case MenuState::SETTINGS:
    if (selectedIndex == 0) {
      selectedTheme = (selectedTheme + 1) % 3;
      setState(MenuState::SETTINGS); // Odlsiwież tekst
      return MenuAction::CHANGE_THEME;
    }
    if (selectedIndex == 1) {
      setState(MenuState::MAIN_MENU);
      return MenuAction::NONE;
    }
    break;
  }
  return MenuAction::NONE;
}

void Menu::handleEvent(const sf::Event &event) {
  // Logic handled in GameEngine for simpler controller/keyboard sync
}

int Menu::checkDifficultyClick(float mouseX, float mouseY) const {
  sf::Vector2f mousePos(mouseX, mouseY);
  if (confirmButton.getGlobalBounds().contains(mousePos))
    return 1;
  if (backButton.getGlobalBounds().contains(mousePos))
    return 2;
  return 0;
}

void Menu::render(sf::RenderWindow &window) const {
  // Półprzezroczyste tło
  sf::RectangleShape background(sf::Vector2f(800, 850));
  background.setFillColor(sf::Color(0, 0, 0, 200));
  window.draw(background);

  if (currentState == MenuState::HIGH_SCORES) {
    sf::Text title;
    title.setFont(font);
    title.setString("HIGH SCORES");
    title.setCharacterSize(50);
    title.setFillColor(sf::Color::Yellow);
    auto tb = title.getLocalBounds();
    title.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
    title.setPosition(400.f, 90.f);
    window.draw(title);

    float y = 160.f;
    if (highScores.empty()) {
      sf::Text empty;
      empty.setFont(font);
      empty.setString("No scores yet");
      empty.setCharacterSize(26);
      empty.setFillColor(sf::Color::White);
      auto eb = empty.getLocalBounds();
      empty.setOrigin(eb.left + eb.width / 2.f, eb.top + eb.height / 2.f);
      empty.setPosition(400.f, y);
      window.draw(empty);
    } else {
      for (size_t i = 0; i < highScores.size() && i < 10; i++) {
        sf::Text line;
        line.setFont(font);
        line.setCharacterSize(26);
        line.setFillColor(sf::Color::White);
        line.setString(std::to_string(i + 1) + ". " + highScores[i].first +
                       " - " + std::to_string(highScores[i].second));
        auto lb = line.getLocalBounds();
        line.setOrigin(lb.left + lb.width / 2.f, lb.top + lb.height / 2.f);
        line.setPosition(400.f, y + (float)i * 38.f);
        window.draw(line);
      }
    }

    sf::Text hint;
    hint.setFont(font);
    hint.setString("ESC / Enter - Back");
    hint.setCharacterSize(18);
    hint.setFillColor(sf::Color(170, 170, 170));
    auto hb = hint.getLocalBounds();
    hint.setOrigin(hb.left + hb.width / 2.f, hb.top + hb.height / 2.f);
    hint.setPosition(400.f, 750.f); // Moved down for taller window
    window.draw(hint);
    return;
  }

  window.draw(titleText);

  if (currentState == MenuState::DIFFICULTY_SELECTION) {
    sf::Text leftCopy = leftArrowText;
    sf::Text rightCopy = rightArrowText;
    sf::Text diffCopy = difficultyText;
    sf::Text confirmCopy = confirmButton;
    sf::Text backCopy = backButton;

    if (selectedDifficultyElement == 0) {
      leftCopy.setFillColor(sf::Color::Yellow);
      diffCopy.setFillColor(sf::Color::Yellow);
      rightCopy.setFillColor(sf::Color::Yellow);
    } else if (selectedDifficultyElement == 1) { // Back
      backCopy.setFillColor(sf::Color::Yellow);
    } else if (selectedDifficultyElement == 2) { // START/Confirm
      confirmCopy.setFillColor(sf::Color::Yellow);
    }

    window.draw(leftCopy);
    window.draw(diffCopy);
    window.draw(rightCopy);
    window.draw(backCopy);
    window.draw(confirmCopy);
    return;
  }

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

void Menu::showGameOver(int scoreValue, bool isHighScore) {
  setState(MenuState::GAME_OVER);
}

void Menu::setTheme(const ColorTheme &theme) { currentTheme = theme; }
