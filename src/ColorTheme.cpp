#include "ColorTheme.h"

ColorTheme CLASSIC_THEME{
    sf::Color(30, 30, 30), // background
    sf::Color(50, 50, 50), // panel
    sf::Color::White,      // text
    sf::Color::Yellow,     // highlight

    sf::Color::Cyan,       // I
    sf::Color::Yellow,     // O
    sf::Color::Magenta,    // T
    sf::Color::Green,      // S
    sf::Color::Red,        // Z
    sf::Color::Blue,       // J
    sf::Color(255, 165, 0) // L
};

ColorTheme DARK_THEME{
    sf::Color(18, 18, 22),    // background
    sf::Color(34, 34, 42),    // panel
    sf::Color(230, 230, 235), // text
    sf::Color(255, 200, 70),  // highlight

    sf::Color(70, 160, 255),  // I  ch³odny niebieski
    sf::Color(245, 215, 80),  // O  bursztynowy
    sf::Color(190, 120, 255), // T  fiolet
    sf::Color(90, 215, 150),  // S  miêtowy
    sf::Color(255, 95, 120),  // Z  malinowy
    sf::Color(95, 120, 255),  // J  granatowo-niebieski
    sf::Color(255, 155, 80)   // L  pomarañcz
};

ColorTheme NEON_THEME{
    sf::Color(6, 0, 14),      // background
    sf::Color(20, 0, 38),     // panel
    sf::Color(235, 245, 255), // text
    sf::Color(255, 0, 200),   // highlight

    sf::Color(0, 255, 240), // I  neon cyan
    sf::Color(255, 255, 0), // O  neon yellow
    sf::Color(255, 0, 200), // T  neon magenta
    sf::Color(0, 255, 90),  // S  neon green
    sf::Color(255, 40, 0),  // Z  neon red-orange
    sf::Color(0, 140, 255), // J  neon blue
    sf::Color(255, 120, 0)  // L  neon orange
};
