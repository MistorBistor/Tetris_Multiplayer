#include <iostream>

#include "GameEngine.h"

int main() {
  try {
    GameEngine engine;

    engine.initialize();
    engine.run();
    engine.shutdown();

    return 0;
  } catch (const std::exception &e) {
    std::cerr << "Wystąpił błąd: " << e.what() << '\n';

    return 1;
  }
}
