#ifndef SCORE_HPP
#define SCORE_HPP
#include "ColorTheme.h"
#include <SFML/Graphics.hpp>

class Score {
private:
  int currentScore;
  int level;
  int linesCleared;
  int highScore;
  int comboCount;

  sf::Font font;

  float gameTimeSeconds = 0.0f;

  ColorTheme currentTheme;

public:
  Score();
  ~Score();

  void initialize();

  void addScore(int lines, int currentLevel);

  void resetCombo();

  void increaseLevel();
  float getFallSpeed() const;
  void reset();

  bool isNewHighScore() const;
  void saveHighScore() const;
  void loadHighScore();

  int getCurrentScore() const { return currentScore; }
  int getLevel() const { return level; }
  int getLinesCleared() const { return linesCleared; }
  int getHighScore() const { return highScore; }
  int getComboCount() const { return comboCount; }

  void render(sf::RenderWindow &window) const;

  void setGameTimeSeconds(float sec) { gameTimeSeconds = sec; }

  void setTheme(const ColorTheme &theme);
};
#endif