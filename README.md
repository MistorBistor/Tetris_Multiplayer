# Tetris w C++ z SFML

Struktura projektu dla klasycznej gry Tetris w C++ z wykorzystaniem biblioteki SFML.

## Opis

Tetris to klasyczna gra logiczna, w której gracz układa spadające klocki (tetromino) w linie. Celem gry jest uzyskanie jak największej liczby punktów poprzez usuwanie zapełnionych linii. Gra przyspiesza wraz ze wzrostem poziomu trudności.

### Planowane funkcjonalności

- Klasyczna rozgrywka Tetris z siedmioma typami klocków
- System punktacji z poziomami trudności
- Menu główne, pauza i ekran końca gry
- Zapis i wczytywanie najwyższego wyniku
- Możliwość zapisu i wznowienia gry

## Technologie

- **Język programowania**: C++17
- **Biblioteka graficzna**: SFML 2.5.1
  - sfml-system
  - sfml-window
  - sfml-graphics
  - sfml-audio
- **System budowania**: CMake 3.10+
- **Licencja**: MIT

## Instalacja

### Wymagania

- Kompilator C++ z obsługą C++17 (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.10 lub nowszy
- Biblioteka SFML 2.5.1

### Instalacja SFML

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install libsfml-dev
```

#### macOS (Homebrew)
```bash
brew install sfml
```

#### Windows
Pobierz SFML 2.5.1 z oficjalnej strony: https://www.sfml-dev.org/download.php
i skonfiguruj zgodnie z dokumentacją.

## Kompilacja

### Linux/macOS

```bash
# Sklonuj repozytorium
git clone https://github.com/meetinjp/podstawy-informatyki-gra.git
cd podstawy-informatyki-gra

# Utwórz katalog build i skompiluj projekt
mkdir -p build
cd build
cmake ..
make

# Uruchom grę
./tetris
```

### Windows (Visual Studio)

```bash
# Sklonuj repozytorium
git clone https://github.com/meetinjp/podstawy-informatyki-gra.git
cd podstawy-informatyki-gra

# Utwórz katalog build i wygeneruj projekt
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"

# Otwórz wygenerowany plik .sln w Visual Studio i skompiluj projekt
```

## Struktura projektu

```
podstawy-informatyki-gra/
├── src/              # Pliki źródłowe (.cpp)
│   ├── main.cpp
│   ├── GameEngine.cpp
│   ├── Board.cpp
│   ├── Tetromino.cpp
│   ├── Score.cpp
│   ├── Menu.cpp
│   └── FileManager.cpp
├── include/          # Pliki nagłówkowe (.hpp)
│   ├── GameEngine.hpp
│   ├── Board.hpp
│   ├── Tetromino.hpp
│   ├── Score.hpp
│   ├── Menu.hpp
│   └── FileManager.hpp
├── resources/        # Zasoby (czcionki, tekstury, dźwięki)
├── docs/             # Dokumentacja
├── build/            # Katalog budowania (generowany)
├── CMakeLists.txt    # Konfiguracja CMake
├── .gitignore
├── LICENSE           # Licencja MIT
└── README.md         # Ten plik
```

## Moduły gry

### GameEngine (Silnik gry)
Główny moduł zarządzający pętlą gry, zdarzeniami i koordynacją innych modułów.

### Board (Plansza)
Zarządza siatką planszy gry, wykrywaniem kolizji i usuwaniem zapełnionych linii.

### Tetromino (Klocki)
Reprezentuje klocki Tetris, ich kształty, kolory i rotacje.

### Score (Punktacja)
System punktacji, poziomów trudności i rekordów.

### Menu (Menu)
Menu główne, menu pauzy i ekran końca gry.

### FileManager (Zarządzanie plikami)
Zapis i odczyt stanu gry, najwyższych wyników i ustawień.

## Autorzy

- Zespół podstawy-informatyki-gra

## Licencja

Ten projekt jest licencjonowany na licencji MIT - szczegóły w pliku [LICENSE](LICENSE).

## Status projektu

🚧 **Projekt w fazie rozwoju** - Szkielety klas zostały utworzone, implementacja funkcjonalności jest w toku.

### TODO

- [ ] Implementacja silnika gry i głównej pętli
- [ ] Implementacja logiki planszy i wykrywania kolizji
- [ ] Implementacja klocków Tetris i ich rotacji
- [ ] Implementacja systemu punktacji
- [ ] Implementacja menu i interfejsu użytkownika
- [ ] Implementacja zapisu i odczytu plików
- [ ] Dodanie zasobów (czcionki, tekstury, dźwięki)
- [ ] Testy i debugowanie
