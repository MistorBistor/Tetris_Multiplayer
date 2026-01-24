#include "Score.h"

Score::Score() : currentScore(0), level(1), linesCleared(0), highScore(0) {
  // TODO: Inicjalizacja zmiennych
}

Score::~Score() {
  // TODO: Cleanup jeśli potrzebne
}

void Score::initialize() {
  // TODO: Załadować czcionkę z pliku
  // TODO: Wczytać najwyższy wynik
  loadHighScore();
}

void Score::addScore(int lines) {
  // TODO: Obliczyć punkty według wzoru Tetris:
  //       1 linia:  40 * (level + 1)
  //       2 linie: 100 * (level + 1)
  //       3 linie: 300 * (level + 1)
  //       4 linie: 1200 * (level + 1)
  // TODO: Dodać punkty do currentScore
  // TODO: Zwiększyć linesCleared
  // TODO: Sprawdzić czy należy zwiększyć poziom (co 10 linii)
}

void Score::increaseLevel() { level++; }

float Score::getFallSpeed() const {
  // TODO: Zwrócić czas spadania w sekundach na podstawie poziomu
  // TODO: Przykład: 1.0 - (level * 0.1) z minimum 0.1
  return 1.0f;
}

void Score::reset() {
  currentScore = 0;
  level = 1;
  linesCleared = 0;
}

bool Score::isNewHighScore() const { return currentScore > highScore; }

void Score::saveHighScore() const {
  // TODO: Zapisać highScore do pliku
  // TODO: Użyć FileManager lub bezpośrednio std::ofstream
}

void Score::loadHighScore() {
  // TODO: Wczytać highScore z pliku
  // TODO: Użyć FileManager lub bezpośrednio std::ifstream
  // TODO: Jeśli plik nie istnieje, ustawić highScore na 0
}

void Score::render(sf::RenderWindow &window) const {
  // TODO: Utworzyć obiekty sf::Text dla:
  //       - Score: currentScore
  //       - Level: level
  //       - Lines: linesCleared
  //       - High Score: highScore
  // TODO: Ustawić czcionkę, rozmiar i pozycję
  // TODO: Narysować wszystkie teksty
}
