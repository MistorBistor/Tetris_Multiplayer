#include "Menu.h"
#include <algorithm>
#include <iostream>

Menu::Menu()
    : currentState(MenuState::MAIN_MENU), selectedIndex(0),
    selectedDifficulty(1), selectedDifficultyElement(0),
    lobbyNameInput(""), isTypingLobbyName(false), selectedLobbyIndex(0),
    lobbyUISelectedElement(0) {}

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
        menuItems = { "START GAME", "MULTIPLAYER", "HIGH SCORES", "SETTINGS", "EXIT" };  // DODANO MULTIPLAYER
        break;

    case MenuState::PAUSE:
        menuItems = { "RESUME", "RESTART", "MAIN MENU", "EXIT" };
        break;

    case MenuState::SETTINGS: {
        std::string themeName = "CLASSIC";
        if (selectedTheme == 1) themeName = "DARK";
        if (selectedTheme == 2) themeName = "NEON";
        menuItems = { "THEME: " + themeName, "BACK" };
        break;
    }

        // NOWE STANY:
    case MenuState::MULTIPLAYER_MENU:
        menuItems = { "HOST GAME", "JOIN GAME", "BACK" };
        break;

    case MenuState::MULTIPLAYER_HOST:
        // Custom rendering
        lobbyUISelectedElement = 0;
        isTypingLobbyName = false;
        return;

    case MenuState::MULTIPLAYER_LOBBY:
        // Custom rendering
        return;

    case MenuState::MULTIPLAYER_JOIN:
        // Custom rendering
        selectedLobbyIndex = 0;
        return;

    case MenuState::HIGH_SCORES:
        // Nie tworzymy itemTexts dla HIGH_SCORES
        return;

    case MenuState::DIFFICULTY_SELECTION:
        // Nie tworzymy itemTexts dla DIFFICULTY_SELECTION
        return;
    }

    // Generuj teksty dla standardowych menu
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
            selectedDifficultyElement = 2;
        return;
    }

    if (currentState == MenuState::MULTIPLAYER_HOST) {
        lobbyUISelectedElement--;
        if (lobbyUISelectedElement < 0)
            lobbyUISelectedElement = 2;
        return;
    }

    if (currentState == MenuState::MULTIPLAYER_JOIN) {
        selectedIndex--;
        if (selectedIndex < 0)
            selectedIndex = (int)availableLobbies.size(); // +1 for back button
        return;
    }

    if (currentState == MenuState::MULTIPLAYER_LOBBY) {
        selectedIndex--;
        if (selectedIndex < 0)
            selectedIndex = 1; // Start/Quit toggle
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

    if (currentState == MenuState::MULTIPLAYER_HOST) {
        lobbyUISelectedElement++;
        if (lobbyUISelectedElement > 2)
            lobbyUISelectedElement = 0;
        return;
    }

    if (currentState == MenuState::MULTIPLAYER_JOIN) {
        selectedIndex++;
        if (selectedIndex > (int)availableLobbies.size())
            selectedIndex = 0;
        return;
    }

    if (currentState == MenuState::MULTIPLAYER_LOBBY) {
        selectedIndex++;
        if (selectedIndex > 1)
            selectedIndex = 0;
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
        if (selectedIndex == 0) return MenuAction::START_GAME;
        if (selectedIndex == 1) return MenuAction::MULTIPLAYER;  // NOWE
        if (selectedIndex == 2) return MenuAction::HIGH_SCORES;
        if (selectedIndex == 3) return MenuAction::SETTINGS;
        if (selectedIndex == 4) return MenuAction::EXIT;
        break;

    case MenuState::PAUSE:
        if (selectedIndex == 0) return MenuAction::RESUME;
        if (selectedIndex == 1) return MenuAction::RESTART;
        if (selectedIndex == 2) return MenuAction::MAIN_MENU;
        if (selectedIndex == 3) return MenuAction::EXIT;
        break;

    case MenuState::SETTINGS:
        if (selectedIndex == 0) {
            selectedTheme = (selectedTheme + 1) % 3;
            setState(MenuState::SETTINGS);
            return MenuAction::CHANGE_THEME;
        }
        if (selectedIndex == 1) {
            setState(MenuState::MAIN_MENU);
            return MenuAction::NONE;
        }
        break;

        // NOWE:
    case MenuState::MULTIPLAYER_MENU:
        if (selectedIndex == 0) return MenuAction::HOST_GAME;
        if (selectedIndex == 1) return MenuAction::JOIN_GAME;
        if (selectedIndex == 2) {
            setState(MenuState::MAIN_MENU);
            return MenuAction::NONE;
        }
        break;

    case MenuState::MULTIPLAYER_HOST:
        if (lobbyUISelectedElement == 1) {  // Create button
            return MenuAction::HOST_GAME;
        }
        else if (lobbyUISelectedElement == 2) {  // Back button
            setState(MenuState::MULTIPLAYER_MENU);
            return MenuAction::NONE;
        }
        break;

    case MenuState::MULTIPLAYER_LOBBY:
        if (selectedIndex == 0) {  // Start Game
            return MenuAction::START_MULTIPLAYER;
        }
        else if (selectedIndex == 1) {  // Quit Lobby
            return MenuAction::QUIT_LOBBY;
        }
        break;

    case MenuState::MULTIPLAYER_JOIN:
        if (selectedIndex < (int)availableLobbies.size()) {
            selectedLobbyIndex = selectedIndex;
            return MenuAction::JOIN_SELECTED_LOBBY;
        }
        else {
            // Back button
            setState(MenuState::MULTIPLAYER_MENU);
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

  // Renderowanie ekranów multiplayer
  if (currentState == MenuState::MULTIPLAYER_HOST) {
      renderMultiplayerHost(window);
      return;
  }

  if (currentState == MenuState::MULTIPLAYER_LOBBY) {
      // To będzie renderowane z GameEngine, który przekaże playerCount i lobbyName
      return;
  }

  if (currentState == MenuState::MULTIPLAYER_JOIN) {
      renderMultiplayerJoin(window);
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

  // Standardowe menu
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

void Menu::setAvailableLobbies(const std::vector<LobbyInfo>& lobbies) {
    availableLobbies = lobbies;
    selectedLobbyIndex = 0;
    selectedIndex = 0;
    std::cout << "[Menu] Załadowano " << lobbies.size() << " lobby\n";
}

void Menu::addCharToLobbyName(char c) {
    if (lobbyNameInput.length() < 20) {  // Max 20 znaków
        lobbyNameInput += c;
    }
}

void Menu::removeCharFromLobbyName() {
    if (!lobbyNameInput.empty()) {
        lobbyNameInput.pop_back();
    }
}

void Menu::renderMultiplayerHost(sf::RenderWindow& window) const {
    // Tytuł
    sf::Text title;
    title.setFont(font);
    title.setString("CREATE LOBBY");
    title.setCharacterSize(40);
    title.setFillColor(sf::Color::Yellow);
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
    title.setPosition(400.f, 150.f);
    window.draw(title);

    // Label "Lobby Name:"
    sf::Text label;
    label.setFont(font);
    label.setString("Lobby Name:");
    label.setCharacterSize(20);
    label.setFillColor(sf::Color::White);
    sf::FloatRect lb = label.getLocalBounds();
    label.setOrigin(lb.left + lb.width / 2.f, lb.top + lb.height / 2.f);
    label.setPosition(400.f, 280.f);
    window.draw(label);

    // Input box
    const float inputWidth = 400.f;
    const float inputHeight = 50.f;
    const float inputX = 200.f;
    const float inputY = 320.f;

    sf::RectangleShape inputBox(sf::Vector2f(inputWidth, inputHeight));
    inputBox.setPosition(inputX, inputY);
    inputBox.setFillColor(sf::Color(40, 40, 60));
    inputBox.setOutlineThickness(3);
    inputBox.setOutlineColor(lobbyUISelectedElement == 0 ? sf::Color::Yellow : sf::Color(100, 100, 120));
    window.draw(inputBox);

    // Wpisana nazwa
    sf::Text nameText;
    nameText.setFont(font);
    nameText.setString(lobbyNameInput.empty()
        ? (isTypingLobbyName ? "_" : "My Lobby")
        : lobbyNameInput + (isTypingLobbyName ? "_" : ""));
    nameText.setCharacterSize(20);
    nameText.setFillColor(lobbyNameInput.empty() && !isTypingLobbyName
        ? sf::Color(150, 150, 150)
        : sf::Color::White);
    sf::FloatRect nb = nameText.getLocalBounds();
    nameText.setOrigin(nb.left + nb.width / 2.f, nb.top + nb.height / 2.f);
    nameText.setPosition(400.f, inputY + inputHeight / 2.f);
    window.draw(nameText);

    // Przyciski
    const float buttonWidth = 180.f;
    const float buttonHeight = 50.f;
    const float buttonY = 450.f;
    const float spacing = 40.f;

    const float totalWidth = buttonWidth * 2 + spacing;
    const float startX = 400.f - totalWidth / 2.f;

    // Create button
    sf::RectangleShape createBtn(sf::Vector2f(buttonWidth, buttonHeight));
    createBtn.setPosition(startX, buttonY);
    createBtn.setFillColor(lobbyUISelectedElement == 1 ? sf::Color(60, 220, 60) : sf::Color(40, 140, 40));
    createBtn.setOutlineThickness(3);
    createBtn.setOutlineColor(lobbyUISelectedElement == 1 ? sf::Color::Yellow : sf::Color(80, 180, 80));
    window.draw(createBtn);

    sf::Text createText;
    createText.setFont(font);
    createText.setString("CREATE");
    createText.setCharacterSize(20);
    createText.setFillColor(sf::Color::White);
    sf::FloatRect ctb = createText.getLocalBounds();
    createText.setOrigin(ctb.left + ctb.width / 2.f, ctb.top + ctb.height / 2.f);
    createText.setPosition(startX + buttonWidth / 2.f, buttonY + buttonHeight / 2.f);
    window.draw(createText);

    // Back button
    sf::RectangleShape backBtn(sf::Vector2f(buttonWidth, buttonHeight));
    backBtn.setPosition(startX + buttonWidth + spacing, buttonY);
    backBtn.setFillColor(lobbyUISelectedElement == 2 ? sf::Color(220, 60, 60) : sf::Color(140, 40, 40));
    backBtn.setOutlineThickness(3);
    backBtn.setOutlineColor(lobbyUISelectedElement == 2 ? sf::Color::Yellow : sf::Color(180, 80, 80));
    window.draw(backBtn);

    sf::Text backText;
    backText.setFont(font);
    backText.setString("BACK");
    backText.setCharacterSize(20);
    backText.setFillColor(sf::Color::White);
    sf::FloatRect btb = backText.getLocalBounds();
    backText.setOrigin(btb.left + btb.width / 2.f, btb.top + btb.height / 2.f);
    backText.setPosition(startX + buttonWidth + spacing + buttonWidth / 2.f, buttonY + buttonHeight / 2.f);
    window.draw(backText);

    // Instrukcja
    sf::Text hint;
    hint.setFont(font);
    hint.setString("Click input or press Enter to type");
    hint.setCharacterSize(14);
    hint.setFillColor(sf::Color(150, 150, 150));
    sf::FloatRect hb = hint.getLocalBounds();
    hint.setOrigin(hb.left + hb.width / 2.f, hb.top + hb.height / 2.f);
    hint.setPosition(400.f, 560.f);
    window.draw(hint);
}

void Menu::renderMultiplayerLobby(sf::RenderWindow& window, int playerCount, const std::string& lobbyName) const {
    // Tytuł
    sf::Text title;
    title.setFont(font);
    title.setString("LOBBY: " + lobbyName);
    title.setCharacterSize(35);
    title.setFillColor(sf::Color::Yellow);
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
    title.setPosition(400.f, 150.f);
    window.draw(title);

    // Status graczy
    sf::Text playerStatus;
    playerStatus.setFont(font);
    playerStatus.setString("Players: " + std::to_string(playerCount) + " / 2");
    playerStatus.setCharacterSize(28);
    playerStatus.setFillColor(playerCount == 2 ? sf::Color::Green : sf::Color::Yellow);
    sf::FloatRect psb = playerStatus.getLocalBounds();
    playerStatus.setOrigin(psb.left + psb.width / 2.f, psb.top + psb.height / 2.f);
    playerStatus.setPosition(400.f, 250.f);
    window.draw(playerStatus);

    // Komunikat oczekiwania
    if (playerCount < 2) {
        sf::Text waiting;
        waiting.setFont(font);
        waiting.setString("Waiting for opponent...");
        waiting.setCharacterSize(20);
        waiting.setFillColor(sf::Color(200, 200, 200));
        sf::FloatRect wb = waiting.getLocalBounds();
        waiting.setOrigin(wb.left + wb.width / 2.f, wb.top + wb.height / 2.f);
        waiting.setPosition(400.f, 320.f);
        window.draw(waiting);
    }

    // Przyciski
    const float buttonWidth = 200.f;
    const float buttonHeight = 55.f;
    const float buttonY = 450.f;
    const float spacing = 40.f;

    const float totalWidth = buttonWidth * 2 + spacing;
    const float startX = 400.f - totalWidth / 2.f;

    // Start Game button (tylko dla hosta i gdy są 2 graczy)
    sf::RectangleShape startBtn(sf::Vector2f(buttonWidth, buttonHeight));
    startBtn.setPosition(startX, buttonY);

    bool canStart = (playerCount == 2);
    startBtn.setFillColor(selectedIndex == 0
        ? (canStart ? sf::Color(60, 220, 60) : sf::Color(80, 80, 80))
        : (canStart ? sf::Color(40, 140, 40) : sf::Color(60, 60, 60)));
    startBtn.setOutlineThickness(3);
    startBtn.setOutlineColor(selectedIndex == 0 ? sf::Color::Yellow : sf::Color(100, 100, 100));
    window.draw(startBtn);

    sf::Text startText;
    startText.setFont(font);
    startText.setString("START");
    startText.setCharacterSize(22);
    startText.setFillColor(canStart ? sf::Color::White : sf::Color(120, 120, 120));
    sf::FloatRect stb = startText.getLocalBounds();
    startText.setOrigin(stb.left + stb.width / 2.f, stb.top + stb.height / 2.f);
    startText.setPosition(startX + buttonWidth / 2.f, buttonY + buttonHeight / 2.f);
    window.draw(startText);

    // Quit button
    sf::RectangleShape quitBtn(sf::Vector2f(buttonWidth, buttonHeight));
    quitBtn.setPosition(startX + buttonWidth + spacing, buttonY);
    quitBtn.setFillColor(selectedIndex == 1 ? sf::Color(220, 60, 60) : sf::Color(140, 40, 40));
    quitBtn.setOutlineThickness(3);
    quitBtn.setOutlineColor(selectedIndex == 1 ? sf::Color::Yellow : sf::Color(180, 80, 80));
    window.draw(quitBtn);

    sf::Text quitText;
    quitText.setFont(font);
    quitText.setString("QUIT");
    quitText.setCharacterSize(22);
    quitText.setFillColor(sf::Color::White);
    sf::FloatRect qtb = quitText.getLocalBounds();
    quitText.setOrigin(qtb.left + qtb.width / 2.f, qtb.top + qtb.height / 2.f);
    quitText.setPosition(startX + buttonWidth + spacing + buttonWidth / 2.f, buttonY + buttonHeight / 2.f);
    window.draw(quitText);

    // Hint
    sf::Text hint;
    hint.setFont(font);
    hint.setString("Use arrows and Enter to navigate");
    hint.setCharacterSize(14);
    hint.setFillColor(sf::Color(150, 150, 150));
    sf::FloatRect hb = hint.getLocalBounds();
    hint.setOrigin(hb.left + hb.width / 2.f, hb.top + hb.height / 2.f);
    hint.setPosition(400.f, 580.f);
    window.draw(hint);
}

void Menu::renderMultiplayerJoin(sf::RenderWindow& window) const {
    // Tytuł
    sf::Text title;
    title.setFont(font);
    title.setString("JOIN LOBBY");
    title.setCharacterSize(40);
    title.setFillColor(sf::Color::Yellow);
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
    title.setPosition(400.f, 100.f);
    window.draw(title);

    float yPos = 200.f;

    if (availableLobbies.empty()) {
        sf::Text noLobbies;
        noLobbies.setFont(font);
        noLobbies.setString("No lobbies found...");
        noLobbies.setCharacterSize(24);
        noLobbies.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect nlb = noLobbies.getLocalBounds();
        noLobbies.setOrigin(nlb.left + nlb.width / 2.f, nlb.top + nlb.height / 2.f);
        noLobbies.setPosition(400.f, 300.f);
        window.draw(noLobbies);
    }
    else {
        // Lista lobby
        for (size_t i = 0; i < availableLobbies.size(); i++) {
            const LobbyInfo& lobby = availableLobbies[i];

            // Box dla lobby
            sf::RectangleShape lobbyBox(sf::Vector2f(600.f, 60.f));
            lobbyBox.setPosition(100.f, yPos);
            lobbyBox.setFillColor(sf::Color(40, 40, 60));
            lobbyBox.setOutlineThickness(3);
            lobbyBox.setOutlineColor(selectedIndex == (int)i ? sf::Color::Yellow : sf::Color(80, 80, 100));
            window.draw(lobbyBox);

            // Nazwa lobby
            sf::Text lobbyName;
            lobbyName.setFont(font);
            lobbyName.setString(lobby.name);
            lobbyName.setCharacterSize(20);
            lobbyName.setFillColor(sf::Color::White);
            lobbyName.setPosition(120.f, yPos + 10.f);
            window.draw(lobbyName);

            // Liczba graczy
            sf::Text playerCount;
            playerCount.setFont(font);
            playerCount.setString(std::to_string(lobby.playerCount) + "/2");
            playerCount.setCharacterSize(18);
            playerCount.setFillColor(sf::Color::Green);
            playerCount.setPosition(120.f, yPos + 35.f);
            window.draw(playerCount);

            // Przycisk JOIN
            sf::RectangleShape joinBtn(sf::Vector2f(100.f, 40.f));
            joinBtn.setPosition(580.f, yPos + 10.f);
            joinBtn.setFillColor(selectedIndex == (int)i ? sf::Color(60, 220, 60) : sf::Color(40, 140, 40));
            window.draw(joinBtn);

            sf::Text joinText;
            joinText.setFont(font);
            joinText.setString("JOIN");
            joinText.setCharacterSize(16);
            joinText.setFillColor(sf::Color::White);
            sf::FloatRect jtb = joinText.getLocalBounds();
            joinText.setOrigin(jtb.left + jtb.width / 2.f, jtb.top + jtb.height / 2.f);
            joinText.setPosition(630.f, yPos + 30.f);
            window.draw(joinText);

            yPos += 80.f;
        }
    }

    // Back button
    sf::RectangleShape backBtn(sf::Vector2f(200.f, 50.f));
    backBtn.setPosition(300.f, 700.f);
    backBtn.setFillColor(selectedIndex == (int)availableLobbies.size() ? sf::Color(220, 60, 60) : sf::Color(140, 40, 40));
    backBtn.setOutlineThickness(3);
    backBtn.setOutlineColor(selectedIndex == (int)availableLobbies.size() ? sf::Color::Yellow : sf::Color(180, 80, 80));
    window.draw(backBtn);

    sf::Text backText;
    backText.setFont(font);
    backText.setString("BACK");
    backText.setCharacterSize(20);
    backText.setFillColor(sf::Color::White);
    sf::FloatRect btb = backText.getLocalBounds();
    backText.setOrigin(btb.left + btb.width / 2.f, btb.top + btb.height / 2.f);
    backText.setPosition(400.f, 725.f);
    window.draw(backText);

    // Hint
    sf::Text hint;
    hint.setFont(font);
    hint.setString("Press R to refresh");
    hint.setCharacterSize(14);
    hint.setFillColor(sf::Color(150, 150, 150));
    sf::FloatRect hb = hint.getLocalBounds();
    hint.setOrigin(hb.left + hb.width / 2.f, hb.top + hb.height / 2.f);
    hint.setPosition(400.f, 780.f);
    window.draw(hint);
}