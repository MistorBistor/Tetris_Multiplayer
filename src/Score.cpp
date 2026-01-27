#include "Score.h"
#include <iostream>
#include <fstream>

Score::Score() 
    : currentScore(0), level(0), linesCleared(0), highScore(0), comboCount(0) {
  std::cout << "[Score] Konstruktor\n";
}

Score::~Score() {
  std::cout << "[Score] Destruktor\n";
}

void Score::initialize() {
  std::cout << "[Score] Inicjalizacja\n";
  
  // Ładowanie czcionki
  if (!font.loadFromFile("../resources/fonts/SourceSansPro-Regular.otf")) {
    std::cout << "[Score ERROR] Nie udało się załadować czcionki!\n";
    return;
  }
  
  std::cout << "[Score] Czcionka załadowana\n";
  
  // Wczytaj najwyższy wynik
  loadHighScore();
}

/**
 * Dodaje punkty za usunięte linie według systemu:
 * 1 linia: 100 * currentLevel
 * 2 linie: 300 * currentLevel
 * 3 linie: 500 * currentLevel
 * 4 linie: 800 * currentLevel
 * + bonus combo: 50 * comboCount * currentLevel
 */
void Score::addScore(int lines, int currentLevel) {
  if (lines <= 0) {
    return;
  }
  
  // Punkty bazowe za linie
  int basePoints = 0;
  switch (lines) {
    case 1: basePoints = 100; break;
    case 2: basePoints = 300; break;
    case 3: basePoints = 500; break;
    case 4: basePoints = 800; break;
    default: basePoints = 100 * lines; break;
  }
  
  // Mnożymy przez aktualny level
  int linePoints = basePoints * currentLevel;
  
  // Zwiększamy combo
  comboCount++;
  
  // Punkty za combo (50 * combo * level)
  int comboPoints = 50 * comboCount * currentLevel;
  
  // Suma punktów
  int totalPoints = linePoints + comboPoints;
  currentScore += totalPoints;
  linesCleared += lines;
  
  std::cout << "[Score] Usunięto " << lines << " linie. "
            << "Punkty bazowe: " << linePoints 
            << ", Combo x" << comboCount << ": " << comboPoints
            << ", Razem: " << totalPoints 
            << ", Wynik: " << currentScore << "\n";
  
  // Sprawdź czy to nowy rekord
  if (currentScore > highScore) {
    highScore = currentScore;
    std::cout << "[Score] Nowy rekord: " << highScore << "!\n";
  }
}

/**
 * Resetuje combo (wywołaj gdy klocek spadnie bez usuwania linii).
 */
void Score::resetCombo() {
  if (comboCount > 0) {
    std::cout << "[Score] Combo zresetowane (było: " << comboCount << ")\n";
    comboCount = 0;
  }
}

void Score::increaseLevel() {
  level++;
  std::cout << "[Score] Poziom zwiększony do: " << level << "\n";
}

float Score::getFallSpeed() const {
  // Prędkość spadania na podstawie poziomu
  float speed = 1.0f - (level * 0.1f);
  if (speed < 0.1f) speed = 0.1f;
  return speed;
}

void Score::reset() {
  std::cout << "[Score] Reset wyniku\n";
  currentScore = 0;
  level = 0;
  linesCleared = 0;
  comboCount = 0;
}

bool Score::isNewHighScore() const {
  return currentScore > highScore;
}

void Score::saveHighScore() const {
  std::ofstream file("highscore.dat");
  if (file.is_open()) {
    file << highScore;
    file.close();
    std::cout << "[Score] Zapisano rekord: " << highScore << "\n";
  } else {
    std::cout << "[Score ERROR] Nie można zapisać rekordu\n";
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
    std::cout << "[Score] Brak pliku z rekordem, ustawiono na 0\n";
  }
}

/**
 * Renderuje wszystkie panele: Score (góra), Level (dół), Combo (nad siatką).
 */
void Score::render(sf::RenderWindow& window) const {
  const float leftMargin = 60.0f;
  const float rightEdge = 800.0f;  // ZMIENIONE z 720 na 800
  const float panelWidth = 150.0f;
  const float rightMargin = 60.0f;
  
  // Siatka: x=270, szerokość=300, koniec na x=570
  // Panele zaczynają się od x=590 (20px od siatki)
  
  // PANEL SCORE - prawa strona z marginesem
  float scoreX = 590;  // 20px od końca siatki
  
  sf::RectangleShape scorePanel(sf::Vector2f(panelWidth, 100));
  scorePanel.setPosition(scoreX, 50);
  scorePanel.setFillColor(sf::Color(50, 50, 50));
  scorePanel.setOutlineThickness(2);
  scorePanel.setOutlineColor(sf::Color::White);
  window.draw(scorePanel);
  
  sf::Text scoreTitle;
  scoreTitle.setFont(font);
  scoreTitle.setString("SCORE");
  scoreTitle.setCharacterSize(20);
  scoreTitle.setFillColor(sf::Color::Yellow);
  scoreTitle.setPosition(scoreX + 20, 60);
  window.draw(scoreTitle);
  
  sf::Text scoreValue;
  scoreValue.setFont(font);
  scoreValue.setString(std::to_string(currentScore));
  scoreValue.setCharacterSize(24);
  scoreValue.setFillColor(sf::Color::White);
  scoreValue.setPosition(scoreX + 20, 95);
  window.draw(scoreValue);
  
  // PANEL LEVEL - na dole, wyrównany z dołem siatki
  // Siatka kończy się na y=650 (y=50 + 20*30)
  sf::RectangleShape levelPanel(sf::Vector2f(panelWidth, 80));
  levelPanel.setPosition(scoreX, 570);  // 650 - 80 = 570
  levelPanel.setFillColor(sf::Color(50, 50, 50));
  levelPanel.setOutlineThickness(2);
  levelPanel.setOutlineColor(sf::Color::White);
  window.draw(levelPanel);
  
  sf::Text levelTitle;
  levelTitle.setFont(font);
  levelTitle.setString("LEVEL");
  levelTitle.setCharacterSize(20);
  levelTitle.setFillColor(sf::Color::Cyan);
  levelTitle.setPosition(scoreX + 20, 580);
  window.draw(levelTitle);
  
  sf::Text levelValue;
  levelValue.setFont(font);
  levelValue.setString(std::to_string(level));
  levelValue.setCharacterSize(24);
  levelValue.setFillColor(sf::Color::White);
  levelValue.setPosition(scoreX + 20, 610);
  window.draw(levelValue);
  
  // PANEL COMBO - nad siatką (wyśrodkowany)
  if (comboCount > 0) {
    sf::RectangleShape comboPanel(sf::Vector2f(300, 35));
    comboPanel.setPosition(270, 10);
    comboPanel.setFillColor(sf::Color(100, 0, 100, 200));
    comboPanel.setOutlineThickness(2);
    comboPanel.setOutlineColor(sf::Color::Yellow);
    window.draw(comboPanel);
    
    sf::Text comboText;
    comboText.setFont(font);
    comboText.setString("COMBO x" + std::to_string(comboCount) + "!");
    comboText.setCharacterSize(22);
    comboText.setFillColor(sf::Color::Yellow);
    comboText.setPosition(290, 15);
    window.draw(comboText);
  }
}