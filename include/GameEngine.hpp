#ifndef GAME_ENGINE_HPP
#define GAME_ENGINE_HPP

#include <SFML/Graphics.hpp>

#include "Board.hpp"
#include "Menu.hpp"

/**
 * @class GameEngine
 * @brief Główny silnik gry Tetris
 *
 * Klasa odpowiedzialna za zarządzanie główną pętlą gry,
 * obsługę zdarzeń i koordynację wszystkich modułów.
 */
class GameEngine {
 private:
  sf::RenderWindow window;
  bool isRunning;
  // TODO: Dodać zmienne do zarządzania stanem gry (menu, gra, pauza, game over)
  // TODO: Dodać wskaźniki/obiekty do Board, Menu, Score

 public:
  /**
   * @brief Konstruktor silnika gry
   */
  GameEngine();

  /**
   * @brief Destruktor
   */
  ~GameEngine();

  /**
   * @brief Inicjalizacja silnika gry
   * TODO: Załadować zasoby, zainicjalizować komponenty
   */
  void initialize();

  /**
   * @brief Uruchomienie głównej pętli gry
   * TODO: Implementować główną pętlę gry z obsługą zdarzeń, aktualizacją i
   * rysowaniem
   */
  void run();

  /**
   * @brief Obsługa zdarzeń wejściowych
   * TODO: Implementować obsługę klawiatury i innych zdarzeń
   */
  void handleEvents();

  /**
   * @brief Aktualizacja stanu gry
   * TODO: Aktualizować logikę gry, pozycje klocków, sprawdzać kolizje
   */
  void update(float deltaTime);

  /**
   * @brief Renderowanie grafiki
   * TODO: Rysować planszę, klocki, UI, menu
   */
  void render();

  /**
   * @brief Zamknięcie gry i zwolnienie zasobów
   * TODO: Zwalniać zasoby, zapisywać stan gry
   */
  void shutdown();
};

#endif  // GAME_ENGINE_HPP
