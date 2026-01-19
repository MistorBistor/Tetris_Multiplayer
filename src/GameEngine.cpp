#include "../include/GameEngine.hpp"

GameEngine::GameEngine() : isRunning(false) {
  // TODO: Inicjalizacja zmiennych członkowskich
}

GameEngine::~GameEngine() {
  // TODO: Cleanup jeśli potrzebne
}

void GameEngine::initialize() {
  // TODO: Utworzyć okno gry
  // TODO: Załadować zasoby (czcionki, tekstury, dźwięki)
  // TODO: Zainicjalizować komponenty (Board, Menu, Score)
  // TODO: Ustawić isRunning na true
}

void GameEngine::run() {
  // TODO: Główna pętla gry
  // TODO: Obliczać deltaTime
  // while (window.isOpen()) {
  //     handleEvents();
  //     update(deltaTime);
  //     render();
  // }
}

void GameEngine::handleEvents() {
  // TODO: Pobierać zdarzenia z window.pollEvent()
  // TODO: Obsłużyć zamknięcie okna
  // TODO: Przekazać zdarzenia do odpowiednich komponentów (Menu, Game)
}

void GameEngine::update(float deltaTime) {
  // TODO: Aktualizować stan gry w zależności od aktualnego stanu
  // (menu/gra/pauza)
  // TODO: Aktualizować pozycje tetromino
  // TODO: Sprawdzać kolizje
  // TODO: Aktualizować punktację
}

void GameEngine::render() {
  // TODO: Wyczyścić okno
  // TODO: Narysować odpowiednie elementy w zależności od stanu
  // TODO: Wyświetlić zawartość okna
}

void GameEngine::shutdown() {
  // TODO: Zapisać stan gry jeśli potrzebne
  // TODO: Zwolnić zasoby
  // TODO: Zamknąć okno
}
