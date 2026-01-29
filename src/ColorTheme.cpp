#include "ColorTheme.h"

ColorTheme CLASSIC_THEME{
    sf::Color (30,30,30),
    sf::Color (50,50,50),
    sf::Color::White,
    sf::Color::Yellow,

    sf::Color::Cyan,
    sf::Color::Yellow,
    sf::Color::Magenta,
    sf::Color::Green,
    sf::Color::Red,
    sf::Color::Blue,
    sf::Color (255,165,0)
};

ColorTheme DARK_THEME{
    sf::Color (15,15,20),
    sf::Color (40,40,50),
    sf::Color (200,200,200),
    sf::Color (120,180,255),

    sf::Color (100,200,255),
    sf::Color (255,220,120),
    sf::Color (200,120,255),
    sf::Color (120,255,180),
    sf::Color (255,120,120),
    sf::Color (120,120,255),
    sf::Color (255,180,120)
};

ColorTheme NEON_THEME{
    sf::Color::Black,
    sf::Color (20,20,20),
    sf::Color::White,
    sf::Color (0,255,255),

    sf::Color (0,255,255),
    sf::Color (255,255,0),
    sf::Color (255,0,255),
    sf::Color (0,255,0),
    sf::Color (255,0,0),
    sf::Color (0,0,255),
    sf::Color (255,128,0)
};
