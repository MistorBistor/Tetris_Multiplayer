# Tetris Multiplayer

Nowoczesna implementacja gry Tetris z obsługą gry multiplayer, napisana w C++ 17 z wykorzystaniem biblioteki SFML.

## 📋 Spis Treści

- [Technologie](#technologie)
- [Wymagania](#wymagania)
- [Instalacja](#instalacja)
- [Struktura Projektu](#struktura-projektu)
- [Moduły Gry](#moduły-gry)
- [Autorzy](#autorzy)
- [TODO](#todo)

## 🛠️ Technologie

- **Język Programowania**: C++ 17
- **Biblioteka Graficzna**: SFML 2.5.1
- **System Budowania**: CMake 3.10+
- **Standard**: C++17

## 📦 Wymagania

### Systemowe
- C++ kompilator obsługujący C++17 (GCC 7.0+, Clang 5.0+, MSVC 2017+)
- CMake 3.10 lub nowszy

### Biblioteki
- SFML 2.5.1 lub nowsza
  - Obsługa grafiki 2D
  - Obsługa okna aplikacji
  - Obsługa zdarzeń

## 🚀 Instalacja

### Linux (Ubuntu/Debian)

```bash
# Instalacja zależności
sudo apt-get update
sudo apt-get install -y libsfml-dev cmake g++

# Klonowanie repozytorium
git clone https://github.com/MistorBistor/Tetris_Multiplayer.git
cd Tetris_Multiplayer

# Budowanie projektu
mkdir build
cd build
cmake ..
make

# Uruchomienie gry
./tetris_multiplayer
