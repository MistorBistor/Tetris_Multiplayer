#include "GameEngine.h"

#include <ctime>
#include <iostream>
#include <random>
#include <filesystem>

GameEngine::GameEngine () : window (sf::VideoMode (500, 700), "Tetris") {}

void GameEngine::initialize () {
    isRunning = true;
    gameState = GameState::Menu;

    mainMenu.initialize ();
    mainMenu.setState (MenuState::MAIN_MENU);

    currentTetromino = Tetromino (TetrominoType::I);

    loadAudio ();

    fallTimer = 0.0f;
    lockTimer = 0.0f;
    isLocking = false;

    hasTouchedGround = false;
    lockMoveCounter = 0;

    std::cout << "[GameEngine] Initialize -> Menu\n";
}

void GameEngine::run () {
    while (window.isOpen ()) {
        float deltaTime = clock.restart ().asSeconds ();

        handleEvents ();

        if (gameState == GameState::Playing) {
            update (deltaTime);
        }

        render ();
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
                    //  Podczas animacji czyszczenia linii ignorujemy sterowanie klockiem
                    // (ESC nadal działa)
                    if (board.isLineClearAnimating ()) {
                        continue;
                    }

                    handleKeyPress (event.key.code);
                }
            }
        }

        // GAME OVER: dowolny klawisz lub kliknięcie -> powrót do MAIN MENU
        if (gameState == GameState::GameOver) {
            if (event.type == sf::Event::KeyPressed ||
                (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left)) {

                std::cout << "[GameEngine] Game Over -> Main Menu\n";

                board.reset ();
                mainMenu.setState (MenuState::MAIN_MENU);
                gameState = GameState::Menu;

                backgroundMusic.stop (); // na wszelki wypadek
            }

            continue; // nie przepuszczamy eventów dalej
        }
    }
}

void GameEngine::update (float deltaTime) {
    
	

    //  Jeśli trwa animacja czyszczenia linii – gra stoi, tylko animujemy
    if (board.isLineClearAnimating ()) {
        int cleared = board.updateClearAnimation (deltaTime);
        if (cleared > 0) {
            std::cout << "[GameEngine] Cleared lines (after anim): " << cleared << "\n";
            updateLevelAndSpeed (cleared);
            spawnNewTetromino ();
        }
        return;
    }

    fallTimer += deltaTime;

    if (fallTimer >= fallSpeed) {
        fallTimer = 0.0f;

        std::cout << "[GameEngine] Falling, y=" << currentTetromino.getY () << "\n";
        currentTetromino.moveDown ();

        if (checkCollision ()) {
            currentTetromino.moveUp ();

            // Klocek DOTKNĄŁ podłoża (od tego momentu zaczynamy liczyć ruchy)
            if (!hasTouchedGround) {
                hasTouchedGround = true;
                lockMoveCounter = 0;
                std::cout << "[LockDelay] First touch -> start counting moves\n";
                
            }

            if (!isLocking) {
                std::cout << "[GameEngine] Start lock delay\n";
                isLocking = true;
                lockTimer = 0.0f;

                // UWAGA: NIE resetujemy lockMoveCounter tutaj,
                // bo wall kick/rotacje potrafią chwilowo odrywać klocek od ziemi
                // i wtedy licznik by się "odnawiał".

                if (sfxLanded.getStatus () != sf::Sound::Playing)
                    sfxLanded.play ();
            }
        }
        else {
            // Klocek faktycznie spadł w dół.
            // UWAGA: NIE resetujemy lockMoveCounter tutaj (to robi exploit).
            // Lock delay możemy anulować, ale licznik zostaje.

            if (isLocking) {
                std::cout << "[GameEngine] Cancel lock delay\n";
                isLocking = false;
                lockTimer = 0.0f;
            }
        }
    }

    if (isLocking) {

        // Jeśli po kicku klocek NIE stoi już na ziemi, to nie odliczamy lock delay
        // i przede wszystkim nie wolno blokować "w powietrzu".
        if (!isPieceGrounded ()) {
            // NIE resetujemy lockMoveCounter.
            // Po ponownym kontakcie limit nadal obowiązuje.
            isLocking = false;
            lockTimer = 0.0f;
            return;
        }

        // Anti infinite spin – wymuszony lock po X ruchach/rotacjach
        // Liczymy tylko jeśli klocek kiedykolwiek dotknął podłoża.
        if (hasTouchedGround && lockMoveCounter >= maxLockMoves) {
            std::cout << "[LockDelay] Max moves reached -> force lock\n";

            lockTetromino ();

            //  Nie spawnujemy od razu jeśli są linie do animacji
            if (board.startClearAnimation ()) {
                sfxLineClear.play ();
            }
            else {
                spawnNewTetromino ();
            }

            // spawnNewTetromino() i tak resetuje timery/liczniki/flagę
            return;
        }

        lockTimer += deltaTime;

        if (lockTimer >= lockDelay) {
            std::cout << "[GameEngine] Lock piece (time)\n";
            lockTetromino ();

            //  Nie spawnujemy od razu jeśli są linie do animacji
            if (board.startClearAnimation ()) {
                sfxLineClear.play ();
            }
            else {
                spawnNewTetromino ();
            }

            return;
        }
    }
}

void GameEngine::render () {
    window.clear (sf::Color::White);

    if (gameState == GameState::Menu) {
        mainMenu.render (window);
    }
    else if (gameState == GameState::Playing) {
        board.render (window);

        // Podczas animacji czyszczenia linii NIE rysujemy aktywnego klocka,
        // bo został już zapisany do grid w lockTetromino()
        if (!board.isLineClearAnimating ()) {
            renderGhostTetromino ();
            currentTetromino.render (window);
        }
    }
    else if (gameState == GameState::GameOver) {
        board.render (window);
        renderGameOver ();
        
    }

    window.display ();
}

void GameEngine::shutdown () { std::cout << "[GameEngine] Shutdown\n"; }

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

    std::cout << "[GameEngine] Random type=" << randomType << "\n";

    isLocking = false;
    lockTimer = 0.0f;
    lockMoveCounter = 0;
    hasTouchedGround = false;

    currentTetromino = Tetromino (static_cast<TetrominoType>(randomType));

    if (checkCollision ()) {
        std::cout << "[GameEngine] GAME OVER - no space for new piece\n";

        backgroundMusic.stop ();
        sfxGameOver.play ();
        gameState = GameState::GameOver;
        
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

void GameEngine::handleMenuSelection () {
    MenuAction action = mainMenu.handleSelection ();
    std::cout << "[GameEngine] Menu action=" << static_cast<int>(action) << "\n";

    if (action != MenuAction::NONE) {
        sfxMenu.play ();
    }

    switch (action) {
    case MenuAction::START_GAME:
        std::cout << "[Menu] -> Difficulty selection\n";
        mainMenu.setState (MenuState::DIFFICULTY_SELECTION);
        break;

    case MenuAction::CONFIRM_DIFFICULTY:
        std::cout << "[Menu] Start game\n";
        startLevel = mainMenu.getSelectedDifficulty ();
        currentLevel = startLevel;
        totalLinesCleared = 0;

        fallSpeed = getFallSpeedForLevel (currentLevel);

        updateMusicSpeed ();   // tempo muzyki zgodne z levelem startowym

        board.updateClearAnimSpeed (currentLevel);  // tempo animacji zgodne z levelem startowym

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

void GameEngine::renderGameOver () {
    // Przyciemnione tło
    sf::RectangleShape overlay (sf::Vector2f (500, 700));
    overlay.setPosition (0, 0);
    overlay.setFillColor (sf::Color (0, 0, 0, 180));
    
    window.draw (overlay);

    // NAPIS: GAME OVER
    sf::Text gameOverText;
    gameOverText.setFont (mainMenu.getFont ());   // ← ważne
    gameOverText.setString ("GAME OVER");
    gameOverText.setCharacterSize (56);
    gameOverText.setFillColor (sf::Color::Red);
    gameOverText.setStyle (sf::Text::Bold);

    // Wyśrodkowanie
    sf::FloatRect gb = gameOverText.getLocalBounds ();
    gameOverText.setOrigin (gb.left + gb.width / 2.f, gb.top + gb.height / 2.f);
    gameOverText.setPosition (250.f, 260.f);   // wyżej

    window.draw (gameOverText);

    // Press any key...
    sf::Text infoText;
    infoText.setFont (mainMenu.getFont ());
    infoText.setString ("Press any key to return to menu");
    infoText.setCharacterSize (20);
    infoText.setFillColor (sf::Color::White);

    sf::FloatRect ib = infoText.getLocalBounds ();
    infoText.setOrigin (ib.left + ib.width / 2.f, ib.top + ib.height / 2.f);
    infoText.setPosition (250.f, 340.f);       // niżej

    window.draw (infoText);
}

void GameEngine::renderGhostTetromino () {
    // Kopia aktualnego klocka (żeby nie ruszać prawdziwego)
    Tetromino ghost = currentTetromino;

    // Zrzucamy go w dół aż do kolizji
    while (true) {
        ghost.moveDown ();

        // Sprawdzamy kolizję dla ghosta (robimy lokalny check)
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
            ghost.moveUp (); // cofamy o 1 (ostatni legalny)
            break;
        }
    }

    // Jeśli ghost jest dokładnie tam gdzie klocek – nie ma sensu go rysować
    if (ghost.getY () == currentTetromino.getY ()) {
        return;
    }

    // Rysujemy ghosta
    ghost.renderGhost (window); // <- to dodamy w kroku 3
}

void GameEngine::handleKeyPress (sf::Keyboard::Key key) {

    // Mały helper: zwiększamy licznik ruchów, ale tylko gdy klocek
    // już wszedł w fazę lock delay (czyli dotknął ziemi kiedykolwiek).
    auto countLockMoveIfNeeded = [&]() {
        if (hasTouchedGround) {
            lockMoveCounter++;
            std::cout << "[LockDelay] Move counter = " << lockMoveCounter << "\n";
        }
        };

    // Mały helper: resetujemy lockTimer tylko gdy klocek stoi na podłożu,
    // żeby nie dało się “zamrażać” locka w powietrzu.
    auto resetLockTimerIfGrounded = [&]() {
        if (hasTouchedGround && isPieceGrounded ()) {
            lockTimer = 0.0f;
        }
        };

    if (key == sf::Keyboard::Left) {
        std::cout << "[Input] LEFT\n";
        currentTetromino.moveLeft ();

        if (checkCollision ()) {
            std::cout << "[Input] Collision -> undo LEFT\n";
            currentTetromino.moveRight ();
        }
        else {
            // Anti infinite spin – ruch liczymy po "pierwszym kontakcie z ziemią"
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
            // Anti infinite spin – ruch liczymy po "pierwszym kontakcie z ziemią"
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

                // WAŻNE: klocek dotknął ziemi -> od tej chwili liczymy ruchy/rotacje
                hasTouchedGround = true;
                // Nie resetujemy lockMoveCounter tutaj, bo zależy Ci, żeby wall kick
                // nie dawał darmowego “odnowienia”.
            }
            if (sfxLanded.getStatus () != sf::Sound::Playing)
                sfxLanded.play ();
        }
    }
    else if (key == sf::Keyboard::Space) {
        std::cout << "[Input] SPACE (hard drop)\n";
        while (!checkCollision ()) {
            currentTetromino.moveDown ();
        }
        currentTetromino.moveUp ();
        lockTetromino ();
        if (sfxLanded.getStatus () != sf::Sound::Playing)
            sfxLanded.play ();

        //  Nie spawnujemy od razu jeśli są linie do animacji
        if (board.startClearAnimation ()) {
            sfxLineClear.play ();
        }
        else {
            spawnNewTetromino ();
        }
    }
    else if (key == sf::Keyboard::Up) {
        std::cout << "[Input] Gracz naciska: GÓRA (rotacja + wall kick)\n";

        // Zapamiętaj pozycję startową
        const int startX = currentTetromino.getX ();
        const int startY = currentTetromino.getY ();

        // Obrót "na próbę"
        currentTetromino.rotate ();

        // Proste testy wall kick
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

                // Sprawdzamy czy po udanej rotacji klocek nadal "stoi" na podłożu.
                // To jest klucz do tego, żeby wall kick (podbijanie do góry)
                // NIE resetował/nie rozwalał licznika lockMoveCounter.
                const bool groundedAfter = isPieceGrounded ();

                // Anti infinite spin – liczymy rotacje po pierwszym dotknięciu ziemi
                // (nawet jeśli kick na moment podbije w górę).
                countLockMoveIfNeeded ();

                // Udana rotacja resetuje timer lock delay TYLKO jeśli nadal stoimy.
                if (groundedAfter) {
                    lockTimer = 0.0f;
                    std::cout << "[WallKick] Reset lockTimer po rotacji\n";
                }

                // Nie wyłączamy tutaj isLocking/hasTouchedGround,
                // bo to właśnie dawało “odnawianie” licznika przez kick do góry.

                break;
            }
        }

        if (!rotationPlaced) {
            std::cout << "[WallKick] Rotacja zablokowana – cofanie\n";

            resetToStartPos ();

            // Cofnij obrót (3×90° = -90°)
            currentTetromino.rotate ();
            currentTetromino.rotate ();
            currentTetromino.rotate ();
        }
    }
}

bool GameEngine::isPieceGrounded () {
    currentTetromino.moveDown ();
    const bool coll = checkCollision ();
    currentTetromino.moveUp ();
    return coll;
}

float GameEngine::getFallSpeedForLevel (int level) const {
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

void GameEngine::updateLevelAndSpeed (int linesJustCleared) {
    totalLinesCleared += linesJustCleared;

    // klasyczna zasada Tetrisa
    int oldLevel = currentLevel;
    currentLevel = startLevel + (totalLinesCleared / 10);

    fallSpeed = getFallSpeedForLevel (currentLevel);

    std::cout << "[LEVEL] start=" << startLevel
        << " lines=" << totalLinesCleared
        << " level=" << currentLevel
        << " fallSpeed=" << fallSpeed << "\n";

    // Przyspiesz muzykę tylko gdy level poszedł w górę
    if (currentLevel != oldLevel) {
        updateMusicSpeed ();
        board.updateClearAnimSpeed (currentLevel); // przyspiesza animację linii
        std::cout << "[LEVEL UP] " << oldLevel << " -> " << currentLevel << "\n";
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
