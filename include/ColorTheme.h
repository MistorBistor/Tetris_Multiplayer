#pragma once
#include <SFML/Graphics.hpp>

enum class ColorThemeType {
    CLASSIC = 0,
    DARK,
    NEON
};

struct ColorTheme {
    // UI
    sf::Color background;
    sf::Color panel;
    sf::Color text;
    sf::Color highlight;

    // Tetromina
    sf::Color I;
    sf::Color O;
    sf::Color T;
    sf::Color S;
    sf::Color Z;
    sf::Color J;
    sf::Color L;
};
