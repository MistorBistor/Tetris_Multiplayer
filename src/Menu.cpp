#include "Menu.h"

Menu::Menu() : currentState(MenuState::MAIN_MENU), selectedIndex(0) {
  // TODO: Inicjalizacja zmiennych
}

Menu::~Menu() {
  // TODO: Cleanup jeśli potrzebne
}

void Menu::initialize() {
  // TODO: Załadować czcionki
  // TODO: Utworzyć elementy menu dla każdego stanu
  // TODO: Ustawić pozycje i style tekstów
}

void Menu::setState(MenuState state) {
  currentState = state;
  selectedIndex = 0;
  // TODO: Zaktualizować menuItems w zależności od stanu:
  //       MAIN_MENU: ["Start Game", "Continue", "Settings", "Exit"]
  //       PAUSE: ["Resume", "Restart", "Main Menu"]
  //       GAME_OVER: ["Restart", "Main Menu", "Exit"]
}

void Menu::moveUp() {
  selectedIndex--;
  if (selectedIndex < 0) {
    selectedIndex = menuItems.size() - 1;
  }
}

void Menu::moveDown() {
  selectedIndex++;
  if (selectedIndex >= static_cast<int>(menuItems.size())) {
    selectedIndex = 0;
  }
}

int Menu::handleSelection() {
  // TODO: Zwrócić akcję na podstawie currentState i selectedIndex
  // TODO: Można użyć enumów lub stałych dla akcji
  return selectedIndex;
}

void Menu::handleEvent(const sf::Event& event) {
  // TODO: Obsłużyć zdarzenia klawiatury:
  //       - Strzałka w górę: moveUp()
  //       - Strzałka w dół: moveDown()
  //       - Enter: handleSelection()
}

void Menu::render(sf::RenderWindow& window) const {
  // TODO: Narysować tło menu (opcjonalnie półprzezroczyste)
  // TODO: Narysować tytuł w zależności od stanu
  // TODO: Narysować wszystkie opcje menu
  // TODO: Podświetlić wybraną opcję (inny kolor lub rozmiar)
}

void Menu::showGameOver(int score, bool isHighScore) {
  // TODO: Ustawić stan na GAME_OVER
  // TODO: Przygotować teksty z wynikiem końcowym
  // TODO: Jeśli isHighScore, wyświetlić gratulacje
}
