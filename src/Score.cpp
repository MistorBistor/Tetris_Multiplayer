#include "Score.h"
#include <fstream>
#include <iostream>

Score::Score()
    : currentScore(0), level(0), linesCleared(0), highScore(0), comboCount(0) {}

Score::~Score() {}

void Score::initialize() {
  if (!font.loadFromFile(
          "../resources/fonts/Press_Start_2P/PressStart2P-Regular.ttf")) {
    std::cout << "[Score ERROR] Nie można załadować czcionki\n";
  }
  loadHighScore();
}

void Score::addScore(int lines, int currentLevel) {
  int basePoints = 0;
  switch (lines) {
  case 1:
    basePoints = 100;
    break;
  case 2:
    basePoints = 300;
    break;
  case 3:
    basePoints = 500;
    break;
  case 4:
    basePoints = 800;
    break;
  }

  currentScore += basePoints * (currentLevel + 1);
  linesCleared += lines;

  if (currentScore > highScore) {
    highScore = currentScore;
  }

  comboCount++;
}

void Score::resetCombo() { comboCount = 0; }

void Score::increaseLevel() { level++; }

float Score::getFallSpeed() const {
  return 0.5f; // Na razie stała prędkość, można rozbudować
}

void Score::reset() {
  currentScore = 0;
  level = 0;
  linesCleared = 0;
  comboCount = 0;
  gameTimeSeconds = 0.0f;
  loadHighScore();
}

bool Score::isNewHighScore() const { return currentScore > highScore; }

void Score::saveHighScore() const {
  std::ofstream file("highscore.dat");
  if (file.is_open()) {
    file << highScore;
    file.close();
    std::cout << "[Score] Zapisano rekord: " << highScore << "\n";
  }
}

void Score::loadHighScore() {
  std::ifstream file("highscore.dat");
  if (file.is_open()) {
    file >> highScore;
    file.close();
    std::cout << "[Score] Wczytano rekord: " << highScore << "\n";
  } else {
    highScore = 0;
  }
}

/**
 * Renderuje wszystkie panele.
 */
void Score::render(sf::RenderWindow &window) const {
  const float panelWidth = 150.0f;
  const float scoreX = 590; // 20px od końca siatki (siatka kończy się na 570)

  // PANEL SCORE - góra, wyrównany z górą planszy (170)
  sf::RectangleShape scorePanel(sf::Vector2f(panelWidth, 100));
  scorePanel.setPosition(scoreX, 170);
  scorePanel.setFillColor(currentTheme.panel);
  scorePanel.setOutlineThickness(2);
  scorePanel.setOutlineColor(currentTheme.text);
  window.draw(scorePanel);

  sf::Text scoreTitle;
  scoreTitle.setFont(font);
  scoreTitle.setString("SCORE");
  scoreTitle.setCharacterSize(20);
  scoreTitle.setFillColor(currentTheme.highlight);
  scoreTitle.setPosition(scoreX + 20, 180);
  window.draw(scoreTitle);

  sf::Text scoreValue;
  scoreValue.setFont(font);
  scoreValue.setString(std::to_string(currentScore));
  scoreValue.setCharacterSize(24);
  scoreValue.setFillColor(currentTheme.text);
  scoreValue.setPosition(scoreX + 20, 215);
  window.draw(scoreValue);

  // PANEL TIME - środek
  sf::RectangleShape timePanel(sf::Vector2f(panelWidth, 80));
  timePanel.setPosition(scoreX, 430);
  timePanel.setFillColor(currentTheme.panel);
  timePanel.setOutlineThickness(2);
  timePanel.setOutlineColor(currentTheme.text);
  window.draw(timePanel);

  sf::Text timeTitle;
  timeTitle.setFont(font);
  timeTitle.setString("TIME");
  timeTitle.setCharacterSize(20);
  timeTitle.setFillColor(currentTheme.highlight);
  timeTitle.setPosition(scoreX + 20, 440);
  window.draw(timeTitle);

  int total = static_cast<int>(gameTimeSeconds);
  int mm = total / 60;
  int ss = total % 60;
  std::string timeStr =
      std::to_string(mm) + ":" + (ss < 10 ? "0" : "") + std::to_string(ss);

  sf::Text timeValue;
  timeValue.setFont(font);
  timeValue.setString(timeStr);
  timeValue.setCharacterSize(24);
  timeValue.setFillColor(currentTheme.text);
  timeValue.setPosition(scoreX + 20, 470);
  window.draw(timeValue);

  // PANEL LEVEL - dół, wyrównany z dołem planszy (690)
  sf::RectangleShape levelPanel(sf::Vector2f(panelWidth, 80));
  levelPanel.setPosition(scoreX, 690);
  levelPanel.setFillColor(currentTheme.panel);
  levelPanel.setOutlineThickness(2);
  levelPanel.setOutlineColor(currentTheme.text);
  window.draw(levelPanel);

  sf::Text levelTitle;
  levelTitle.setFont(font);
  levelTitle.setString("LEVEL");
  levelTitle.setCharacterSize(20);
  levelTitle.setFillColor(currentTheme.highlight);
  levelTitle.setPosition(scoreX + 20, 700);
  window.draw(levelTitle);

  sf::Text levelValue;
  levelValue.setFont(font);
  levelValue.setString(std::to_string(level));
  levelValue.setCharacterSize(24);
  levelValue.setFillColor(currentTheme.text);
  levelValue.setPosition(scoreX + 20, 730);
  window.draw(levelValue);

  // PANEL COMBO - nad siatką
  if (comboCount > 0) {
    sf::RectangleShape comboPanel(sf::Vector2f(300, 35));
    comboPanel.setPosition(270, 10);
    comboPanel.setFillColor(currentTheme.panel);
    comboPanel.setOutlineThickness(2);
    comboPanel.setOutlineColor(currentTheme.highlight);
    window.draw(comboPanel);

    sf::Text comboText;
    comboText.setFont(font);
    comboText.setString("COMBO x" + std::to_string(comboCount) + "!");
    comboText.setCharacterSize(22);
    comboText.setFillColor(currentTheme.highlight);
    comboText.setPosition(290, 15);
    window.draw(comboText);
  }
}

void Score::setTheme(const ColorTheme &theme) { currentTheme = theme; }
