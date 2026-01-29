#include "Menu.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <utility>

// MENU SFX (tylko w tym pliku – nie ruszamy Menu.h)
static sf::SoundBuffer sbMenuMove;
static sf::SoundBuffer sbMenuSelect;
static sf::SoundBuffer sbMenuBack;

static sf::Sound sfxMenuMove;
static sf::Sound sfxMenuSelect;
static sf::Sound sfxMenuBack;

Menu::Menu ()
    : currentState (MenuState::MAIN_MENU),
    selectedIndex (0),
    selectedDifficulty (0),
    selectedDifficultyElement (2),
    selectedTheme (0)  // <-- WAŻNE: inicjalizacja
{
    std::cout << "Menu - konstruktor\n";
}

Menu::~Menu () {
    std::cout << "Menu - destruktor\n";
}

/**
 * Inicjalizacja menu - ładuje czcionkę i tworzy teksty.
 */
void Menu::initialize () {
    std::cout << "Menu - inicjalizacja\n";

    // Ładowanie czcionki
    if (!font.loadFromFile ("../resources/fonts/SourceSansPro-Regular.otf")) {
        std::cout << "[ERROR] Nie udało się załadować czcionki!\n";
        return;
    }

    // MENU SFX
    if (!sbMenuMove.loadFromFile ("../resources/audio/Tetris_move_piece.ogg")) {
        std::cout << "[ERROR] Nie udało się załadować menu move sfx!\n";
    }
    if (!sbMenuSelect.loadFromFile ("../resources/audio/Tetris_rotate_piece.ogg")) {
        std::cout << "[ERROR] Nie udało się załadować menu select sfx!\n";
    }
    if (!sbMenuBack.loadFromFile ("../resources/audio/Tetris_line_clear.ogg")) {
        std::cout << "[ERROR] Nie udało się załadować menu back sfx!\n";
    }

    sfxMenuMove.setBuffer (sbMenuMove);
    sfxMenuSelect.setBuffer (sbMenuSelect);
    sfxMenuBack.setBuffer (sbMenuBack);

    sfxMenuMove.setVolume (60.f);
    sfxMenuSelect.setVolume (70.f);
    sfxMenuBack.setVolume (70.f);

    // Konfiguracja tytułu
    titleText.setFont (font);
    titleText.setCharacterSize (TITLE_SIZE);
    titleText.setFillColor (sf::Color::White);

    // Konfiguracja tekstów dla wyboru trudności
    difficultyText.setFont (font);
    difficultyText.setCharacterSize (40);
    difficultyText.setFillColor (sf::Color::White);

    leftArrowText.setFont (font);
    leftArrowText.setString ("<");
    leftArrowText.setCharacterSize (50);
    leftArrowText.setFillColor (sf::Color::White);

    rightArrowText.setFont (font);
    rightArrowText.setString (">");
    rightArrowText.setCharacterSize (50);
    rightArrowText.setFillColor (sf::Color::White);

    confirmButton.setFont (font);
    confirmButton.setString ("Confirm");
    confirmButton.setCharacterSize (30);
    confirmButton.setFillColor (sf::Color::White);

    backButton.setFont (font);
    backButton.setString ("Back");
    backButton.setCharacterSize (30);
    backButton.setFillColor (sf::Color::White);

    setState (MenuState::MAIN_MENU);
}

/**
 * Ustawia stan menu i aktualizuje listę opcji.
 */
void Menu::setState (MenuState state) {
    currentState = state;
    selectedIndex = 0;

    menuItems.clear ();
    itemTexts.clear ();

    switch (currentState) {
    case MenuState::MAIN_MENU:
        titleText.setString ("TETRIS");
        menuItems = { "Play", "High Scores", "Settings", "Quit" };
        break;

    case MenuState::HIGH_SCORES:
        titleText.setString ("HIGH SCORES");
        menuItems = {}; // brak klikalnych pozycji
        break;

    case MenuState::PAUSE:
        titleText.setString ("PAUSED");
        menuItems = { "Resume", "Restart", "Main Menu" };
        break;

    case MenuState::GAME_OVER:
        titleText.setString ("GAME OVER");
        menuItems = { "Restart", "Main Menu", "Quit" };
        break;

    case MenuState::SETTINGS:
        titleText.setString ("SETTINGS");
        menuItems = {
            std::string ("Color Theme: ") + (selectedTheme == 0 ? "Classic" : selectedTheme == 1 ? "Dark" : "Neon"),
            "Back"
        };
        break;

    case MenuState::DIFFICULTY_SELECTION:
        titleText.setString ("SELECT DIFFICULTY");
        selectedDifficultyElement = 2; // start na Confirm
        updateDifficultyDisplay ();
        break;

    default:
        break;
    }

    // wycentruj tytuł
    sf::FloatRect titleBounds = titleText.getLocalBounds ();
    titleText.setOrigin (titleBounds.left + titleBounds.width / 2.0f,
        titleBounds.top + titleBounds.height / 2.0f);
    titleText.setPosition (WINDOW_WIDTH / 2.0f, 150.0f);

    // Tworzenie tekstów opcji (poza DIFFICULTY_SELECTION)
    if (currentState != MenuState::DIFFICULTY_SELECTION) {
        int startY = 300;

        for (size_t i = 0; i < menuItems.size (); i++) {
            sf::Text itemText;
            itemText.setFont (font);
            itemText.setString (menuItems[i]);
            itemText.setCharacterSize (ITEM_SIZE);
            itemText.setFillColor (sf::Color::White);

            sf::FloatRect bounds = itemText.getLocalBounds ();
            itemText.setOrigin (bounds.left + bounds.width / 2.0f,
                bounds.top + bounds.height / 2.0f);
            itemText.setPosition (WINDOW_WIDTH / 2.0f, startY + (float)i * ITEM_SPACING);

            itemTexts.push_back (itemText);
        }
    }
}

/**
 * Aktualizuje wyświetlany tekst poziomu trudności.
 */
void Menu::updateDifficultyDisplay () {
    difficultyText.setString ("Level: " + std::to_string (selectedDifficulty));

    sf::FloatRect diffBounds = difficultyText.getLocalBounds ();
    difficultyText.setOrigin (diffBounds.left + diffBounds.width / 2.0f,
        diffBounds.top + diffBounds.height / 2.0f);
    difficultyText.setPosition (WINDOW_WIDTH / 2.0f, 350.0f);

    sf::FloatRect leftBounds = leftArrowText.getLocalBounds ();
    leftArrowText.setOrigin (leftBounds.left + leftBounds.width / 2.0f,
        leftBounds.top + leftBounds.height / 2.0f);
    leftArrowText.setPosition (WINDOW_WIDTH / 2.0f - 150.0f, 350.0f);

    sf::FloatRect rightBounds = rightArrowText.getLocalBounds ();
    rightArrowText.setOrigin (rightBounds.left + rightBounds.width / 2.0f,
        rightBounds.top + rightBounds.height / 2.0f);
    rightArrowText.setPosition (WINDOW_WIDTH / 2.0f + 150.0f, 350.0f);

    // Back po lewej
    sf::FloatRect backBounds = backButton.getLocalBounds ();
    backButton.setOrigin (backBounds.left + backBounds.width / 2.0f,
        backBounds.top + backBounds.height / 2.0f);
    backButton.setPosition (WINDOW_WIDTH / 2.0f - 80.0f, 500.0f);

    // Confirm po prawej
    sf::FloatRect confirmBounds = confirmButton.getLocalBounds ();
    confirmButton.setOrigin (confirmBounds.left + confirmBounds.width / 2.0f,
        confirmBounds.top + confirmBounds.height / 2.0f);
    confirmButton.setPosition (WINDOW_WIDTH / 2.0f + 80.0f, 500.0f);
}

void Menu::increaseDifficulty () {
    selectedDifficulty++;
    if (selectedDifficulty > MAX_DIFFICULTY) selectedDifficulty = MIN_DIFFICULTY;
    updateDifficultyDisplay ();
}

void Menu::decreaseDifficulty () {
    selectedDifficulty--;
    if (selectedDifficulty < MIN_DIFFICULTY) selectedDifficulty = MAX_DIFFICULTY;
    updateDifficultyDisplay ();
}

float Menu::getDifficultySpeed () const {
    const int framesPerDrop[] = { 48,43,38,33,28,23,18,13,8,6 };
    if (selectedDifficulty >= 0 && selectedDifficulty <= 9)
        return framesPerDrop[selectedDifficulty] / 60.0f;
    return 0.8f;
}

void Menu::moveUp () {
    if (menuItems.empty ()) return; // <-- ważne dla HIGH_SCORES
    selectedIndex--;
    if (selectedIndex < 0) selectedIndex = (int)menuItems.size () - 1;
}

void Menu::moveDown () {
    if (menuItems.empty ()) return; // <-- ważne dla HIGH_SCORES
    selectedIndex++;
    if (selectedIndex >= (int)menuItems.size ()) selectedIndex = 0;
}

MenuAction Menu::handleSelection () {
    // DIFFICULTY_SELECTION
    if (currentState == MenuState::DIFFICULTY_SELECTION) {
        if (selectedDifficultyElement == 2) return MenuAction::CONFIRM_DIFFICULTY;
        if (selectedDifficultyElement == 1) return MenuAction::MAIN_MENU;

        // element 0 -> przejście na Confirm
        selectedDifficultyElement = 2;
        return MenuAction::NONE;
    }

    switch (currentState) {
    case MenuState::MAIN_MENU:
        if (selectedIndex == 0) return MenuAction::START_GAME;
        if (selectedIndex == 1) return MenuAction::HIGH_SCORES;
        if (selectedIndex == 2) return MenuAction::SETTINGS;
        if (selectedIndex == 3) return MenuAction::EXIT;
        break;

    case MenuState::PAUSE:
        if (selectedIndex == 0) return MenuAction::RESUME;
        if (selectedIndex == 1) return MenuAction::RESTART;
        if (selectedIndex == 2) return MenuAction::MAIN_MENU;
        break;

    case MenuState::GAME_OVER:
        if (selectedIndex == 0) return MenuAction::RESTART;
        if (selectedIndex == 1) return MenuAction::MAIN_MENU;
        if (selectedIndex == 2) return MenuAction::EXIT;
        break;

    case MenuState::SETTINGS:
        if (selectedIndex == 0) return MenuAction::CHANGE_THEME;
        if (selectedIndex == 1) return MenuAction::MAIN_MENU;
        break;

    default:
        break;
    }

    return MenuAction::NONE;
}

void Menu::handleEvent (const sf::Event& event) {
    // SETTINGS: zmiana theme lewo/prawo gdy zaznaczona linia 0
    if (currentState == MenuState::SETTINGS) {
        if (event.type == sf::Event::KeyPressed) {
            if ((event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right) && selectedIndex == 0) {
                selectedTheme = (selectedTheme + 1) % 3;

                menuItems[0] = std::string ("Color Theme: ") +
                    (selectedTheme == 0 ? "Classic" : selectedTheme == 1 ? "Dark" : "Neon");

                if (!itemTexts.empty ()) {
                    itemTexts[0].setString (menuItems[0]);
                    sf::FloatRect b = itemTexts[0].getLocalBounds ();
                    itemTexts[0].setOrigin (b.left + b.width / 2.0f, b.top + b.height / 2.0f);
                }

                sfxMenuMove.play ();
            }
        }
        // reszta działa normalnie (Up/Down/Enter)
    }

    // HIGH SCORES: ESC/Enter wraca
    if (currentState == MenuState::HIGH_SCORES) {
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape || event.key.code == sf::Keyboard::Enter) {
                sfxMenuBack.play ();
                setState (MenuState::MAIN_MENU);
            }
        }
        return;
    }

    // DIFFICULTY_SELECTION: własna nawigacja
    if (currentState == MenuState::DIFFICULTY_SELECTION) {
        if (event.type == sf::Event::KeyPressed) {
            if (selectedDifficultyElement == 0) {
                if (event.key.code == sf::Keyboard::Left) { decreaseDifficulty (); sfxMenuMove.play (); }
                else if (event.key.code == sf::Keyboard::Right) { increaseDifficulty (); sfxMenuMove.play (); }
                else if (event.key.code == sf::Keyboard::Down) { selectedDifficultyElement = 2; sfxMenuMove.play (); }
                else if (event.key.code == sf::Keyboard::Enter) { selectedDifficultyElement = 2; sfxMenuSelect.play (); }
            }
            else if (selectedDifficultyElement == 1) { // Back
                if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right) { selectedDifficultyElement = 2; sfxMenuMove.play (); }
                else if (event.key.code == sf::Keyboard::Up) { selectedDifficultyElement = 0; sfxMenuMove.play (); }
                else if (event.key.code == sf::Keyboard::Enter) { sfxMenuBack.play (); }
            }
            else if (selectedDifficultyElement == 2) { // Confirm
                if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::Right) { selectedDifficultyElement = 1; sfxMenuMove.play (); }
                else if (event.key.code == sf::Keyboard::Up) { selectedDifficultyElement = 0; sfxMenuMove.play (); }
                else if (event.key.code == sf::Keyboard::Enter) { sfxMenuSelect.play (); }
            }
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos ((float)event.mouseButton.x, (float)event.mouseButton.y);

            if (leftArrowText.getGlobalBounds ().contains (mousePos)) {
                decreaseDifficulty (); selectedDifficultyElement = 0; sfxMenuMove.play ();
            }
            else if (rightArrowText.getGlobalBounds ().contains (mousePos)) {
                increaseDifficulty (); selectedDifficultyElement = 0; sfxMenuMove.play ();
            }
            else if (difficultyText.getGlobalBounds ().contains (mousePos)) {
                selectedDifficultyElement = 0; sfxMenuMove.play ();
            }
            else if (backButton.getGlobalBounds ().contains (mousePos)) {
                selectedDifficultyElement = 1; sfxMenuBack.play ();
            }
            else if (confirmButton.getGlobalBounds ().contains (mousePos)) {
                selectedDifficultyElement = 2; sfxMenuSelect.play ();
            }
        }

        if (event.type == sf::Event::MouseMoved) {
            sf::Vector2f mousePos ((float)event.mouseMove.x, (float)event.mouseMove.y);

            if (confirmButton.getGlobalBounds ().contains (mousePos)) selectedDifficultyElement = 2;
            else if (backButton.getGlobalBounds ().contains (mousePos)) selectedDifficultyElement = 1;
            else if (leftArrowText.getGlobalBounds ().contains (mousePos) ||
                rightArrowText.getGlobalBounds ().contains (mousePos) ||
                difficultyText.getGlobalBounds ().contains (mousePos)) selectedDifficultyElement = 0;
        }

        return;
    }

    // Standard: klawiatura
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Up) { moveUp (); sfxMenuMove.play (); }
        else if (event.key.code == sf::Keyboard::Down) { moveDown (); sfxMenuMove.play (); }
        else if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Space) {
            sfxMenuSelect.play ();
        }
    }

    // Standard: hover myszą
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos ((float)event.mouseMove.x, (float)event.mouseMove.y);
        for (size_t i = 0; i < itemTexts.size (); i++) {
            if (itemTexts[i].getGlobalBounds ().contains (mousePos)) {
                selectedIndex = (int)i;
                break;
            }
        }
    }

    // Standard: klik myszą
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos ((float)event.mouseButton.x, (float)event.mouseButton.y);
        for (size_t i = 0; i < itemTexts.size (); i++) {
            if (itemTexts[i].getGlobalBounds ().contains (mousePos)) {
                selectedIndex = (int)i;
                sfxMenuSelect.play ();
                break;
            }
        }
    }
}

int Menu::checkDifficultyClick (float mouseX, float mouseY) const {
    sf::Vector2f mousePos (mouseX, mouseY);
    if (confirmButton.getGlobalBounds ().contains (mousePos)) return 1;
    if (backButton.getGlobalBounds ().contains (mousePos)) return 2;
    return 0;
}

/**
 * Renderuje menu na ekranie.
 * UWAGA: bez const, żeby pasowało do Menu.h (typowo)
 */
void Menu::render (sf::RenderWindow& window) const {
    sf::RectangleShape background (sf::Vector2f (800.f, 700.f));
    background.setFillColor (sf::Color (0, 0, 0, 200));
    window.draw (background);

    // HIGH SCORES osobno
    if (currentState == MenuState::HIGH_SCORES) {
        sf::Text title;
        title.setFont (font);
        title.setString ("HIGH SCORES");
        title.setCharacterSize (50);
        title.setFillColor (sf::Color::Yellow);

        auto tb = title.getLocalBounds ();
        title.setOrigin (tb.left + tb.width / 2.f, tb.top + tb.height / 2.f);
        title.setPosition (400.f, 90.f);
        window.draw (title);

        float y = 160.f;

        if (highScores.empty ()) {
            sf::Text empty;
            empty.setFont (font);
            empty.setString ("No scores yet");
            empty.setCharacterSize (26);
            empty.setFillColor (sf::Color::White);

            auto eb = empty.getLocalBounds ();
            empty.setOrigin (eb.left + eb.width / 2.f, eb.top + eb.height / 2.f);
            empty.setPosition (400.f, y);
            window.draw (empty);
        }
        else {
            for (size_t i = 0; i < highScores.size () && i < 10; i++) {
                sf::Text line;
                line.setFont (font);
                line.setCharacterSize (26);
                line.setFillColor (sf::Color::White);

                line.setString (std::to_string (i + 1) + ". " + highScores[i].first + " - " + std::to_string (highScores[i].second));

                auto lb = line.getLocalBounds ();
                line.setOrigin (lb.left + lb.width / 2.f, lb.top + lb.height / 2.f);
                line.setPosition (400.f, y + (float)i * 38.f);

                window.draw (line);
            }
        }

        sf::Text hint;
        hint.setFont (font);
        hint.setString ("ESC / Enter - Back");
        hint.setCharacterSize (18);
        hint.setFillColor (sf::Color (170, 170, 170));

        auto hb = hint.getLocalBounds ();
        hint.setOrigin (hb.left + hb.width / 2.f, hb.top + hb.height / 2.f);
        hint.setPosition (400.f, 650.f);
        window.draw (hint);

        return;
    }

    // reszta stanów
    window.draw (titleText);

    if (currentState == MenuState::DIFFICULTY_SELECTION) {
        sf::Text leftCopy = leftArrowText;
        sf::Text rightCopy = rightArrowText;
        sf::Text diffCopy = difficultyText;
        sf::Text confirmCopy = confirmButton;
        sf::Text backCopy = backButton;

        if (selectedDifficultyElement == 0) {
            leftCopy.setFillColor (sf::Color::Yellow);
            diffCopy.setFillColor (sf::Color::Yellow);
            rightCopy.setFillColor (sf::Color::Yellow);
        }
        else if (selectedDifficultyElement == 1) {
            backCopy.setFillColor (sf::Color::Yellow);
        }
        else if (selectedDifficultyElement == 2) {
            confirmCopy.setFillColor (sf::Color::Yellow);
        }

        window.draw (leftCopy);
        window.draw (diffCopy);
        window.draw (rightCopy);
        window.draw (backCopy);
        window.draw (confirmCopy);
        return;
    }

    for (size_t i = 0; i < itemTexts.size (); i++) {
        sf::Text item = itemTexts[i];
        if ((int)i == selectedIndex) item.setFillColor (sf::Color::Yellow);
        window.draw (item);
    }
}

void Menu::showGameOver (int scoreValue, bool isHighScore) {
    std::cout << "Menu - Game Over, wynik: " << scoreValue << "\n";
    setState (MenuState::GAME_OVER);
    if (isHighScore) std::cout << "Nowy rekord!\n";
}

void Menu::setTheme (const ColorTheme& theme) {
    currentTheme = theme;
}
