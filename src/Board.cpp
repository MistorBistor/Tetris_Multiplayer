#include "Board.hpp"

Board::Board() {
    // TODO: Inicjalizacja zmiennych
}

Board::~Board() {
    // TODO: Cleanup jeśli potrzebne
}

void Board::initialize() {
    // TODO: Zainicjalizować siatkę grid (BOARD_WIDTH x BOARD_HEIGHT)
    // TODO: Wypełnić wszystkie komórki wartością 0 (puste)
}

bool Board::canPlaceTetromino(const Tetromino& tetromino, int x, int y) const {
    // TODO: Pobrać kształt tetromino
    // TODO: Sprawdzić czy każdy blok tetromino:
    //       - mieści się w granicach planszy
    //       - nie nachodzi na zajęte komórki
    // TODO: Zwrócić true jeśli może być umieszczone, false w przeciwnym razie
    return false;
}

void Board::placeTetromino(const Tetromino& tetromino, int x, int y) {
    // TODO: Pobrać kształt i kolor tetromino
    // TODO: Dla każdego bloku tetromino:
    //       - zapisać wartość w grid na odpowiedniej pozycji
}

int Board::clearFullLines() {
    // TODO: Przeszukać wszystkie wiersze od dołu do góry
    // TODO: Dla każdego zapełnionego wiersza:
    //       - usunąć wiersz
    //       - przesunąć wszystkie wiersze powyżej w dół
    // TODO: Zwrócić liczbę usuniętych linii
    return 0;
}

bool Board::isGameOver() const {
    // TODO: Sprawdzić czy górne wiersze są zajęte
    // TODO: Sprawdzić czy można umieścić nowy tetromino
    return false;
}

void Board::clear() {
    // TODO: Wyczyścić wszystkie komórki siatki
}

void Board::render(sf::RenderWindow& window) const {
    // TODO: Dla każdej komórki w grid:
    //       - jeśli zajęta, narysować kolorowy kwadrat
    // TODO: Narysować linie siatki (opcjonalnie)
}
