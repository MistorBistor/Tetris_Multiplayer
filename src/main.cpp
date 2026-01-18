#include "GameEngine.hpp"
#include <iostream>

/**
 * @brief Punkt wejścia aplikacji Tetris
 * 
 * Tworzy i uruchamia silnik gry.
 */
int main() {
    try {
        // TODO: Utworzyć instancję GameEngine
        // GameEngine engine;
        
        // TODO: Zainicjalizować silnik gry
        // engine.initialize();
        
        // TODO: Uruchomić główną pętlę gry
        // engine.run();
        
        // TODO: Zamknąć grę
        // engine.shutdown();
        
        std::cout << "Tetris - Gra w fazie rozwoju" << std::endl;
        std::cout << "TODO: Zaimplementować GameEngine i uruchomić grę" << std::endl;
        
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Błąd: " << e.what() << std::endl;
        return 1;
    }
}
