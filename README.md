# Podstawy informatyki - Gra

Strukturę projektu wygenerował GitHub Copilot na podstawie opisu projektu.

### Planowane funkcjonalności

- Klasyczna rozgrywka Tetris z siedmioma typami klocków (Tetromino)
- System punktacji z POZIOMAMI TRUDNOŚCI
- Menu główne, pauza i ekran końca gry
- Zapis i wczytywanie NAJWYŻSZYCH WYNIKÓW do pliku

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

- Kompilator C++ z obsługą C++17
- CMake 3.10 lub nowszy
- Biblioteka SFML 2.5.1

### Krok po kroku

1. Zainstaluj wymagane narzędzia i biblioteki (CMake, SFML).
2. Sklonuj repozytorium projektu.
3. Wykonaj nastepujące polecenia w terminalu:

   ```bash
   # Utwórz katalog build, skonfiguruj i zbuduj projekt
   mkdir -p build
   cd build
   cmake ..
   make

   # Uruchom grę
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
├── include/          # Pliki nagłówkowe - przede wszystkim deklaracje
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
├── .clang-format     # Konfiguracja formatowania kodu
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

- Zespół programistów:
  - Mateusz Mirczak
  - Kacper Świrkosz
  - Bartłomiej Wiklik

## Licencja

Ten projekt jest licencjonowany na licencji MIT - szczegóły w pliku [LICENSE](LICENSE).

## TODO

- [ ] Implementacja silnika gry i głównej pętli
- [ ] Implementacja logiki planszy i wykrywania kolizji
- [ ] Implementacja klocków Tetris i ich rotacji
- [ ] Implementacja systemu punktacji
- [ ] Implementacja menu i interfejsu użytkownika
- [ ] Implementacja zapisu i odczytu plików
- [ ] Dodanie zasobów (czcionki, tekstury, dźwięki)
- [ ] Testy i debugowanie
