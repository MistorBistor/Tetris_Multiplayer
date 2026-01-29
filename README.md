# Podstawy informatyki - gra

Wstępną strukturę projektu wygenerował GitHub Copilot.

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
├── src/              # Pliki źródłowe
│   ├── main.cpp
│   ├── GameEngine.cpp
│   ├── Board.cpp
│   ├── Tetromino.cpp
│   ├── Score.cpp
│   ├── Menu.cpp
│   └── FileManager.cpp
├── include/          # Pliki nagłówkowe - przede wszystkim deklaracje
│   ├── GameEngine.h
│   ├── Board.h
│   ├── Tetromino.h
│   ├── TetrominoType.h
│   ├── Score.h
│   ├── Menu.h
│   └── FileManager.h
├── resources/         # Zasoby (audio, czcionki)
├── docs/              # Dokumentacja
├── build/             # Katalog budowania (generowany)
├── CMakeLists.txt     # Konfiguracja CMake
├── CMakeSettings.json # Ustawienia CMake dla Visual Studio
├── .clang-format      # Konfiguracja formatowania kodu (styl wg Google)
├── .gitignore
├── LICENSE            # Licencja MIT
└── README.md
```

## Moduły gry

### GameEngine (silnik gry)

Serce gry. Zarządza pętlą gry (game loop), zdarzeniami i koordynacją wszystkich innych modułów.

### Board (plansza)

Plansza, siatka 10x20, na której odbywa się rozgrywka. Wykrywanie kolizji, usuwanie zapełnionych linii.

### Tetromino (klocki)

Reprezentuje klocki Tetris, ich kształty, kolory i rotacje (w sumie 7 rodzajów). Obracanie i przesuwanie klocków.

### Score (punktacja)

System punktacji, poziomów trudności i rekordów. Aktualizacja wyniku i poziomu w trakcie gry.

### Menu

Interfejs menu głównego, menu pauzy i ekran końca gry. Nawigacja i wybór opcji.

### FileManager (zarządzanie plikami)

Zapis i odczyt stanu gry, najwyższych wyników i ustawień.

## Autorzy

- Zespół programistów:
  - Mateusz Mirczak
  - Kacper Świrkosz
  - Bartłomiej Wiklik

## Licencja

Ten projekt jest licencjonowany na licencji MIT - szczegóły w pliku [LICENSE](LICENSE).

## TODO (podział prac)

- [x] Implementacja silnika gry i głównej pętli
- [x] Implementacja logiki planszy (siatka, renderowanie)
- [x] Implementacja wykrywania kolizji (dno, boki)
- [x] Implementacja podstawowego klocka (I-shape)
- [x] Implementacja grawitacji i spadania
- [x] Implementacja blokowania klocka na planszy
- [x] Implementacja ekranu Game Over

---

- [x] Implementacja wszystkich 7 kształtów klocków (I, O, T, S, Z, J, L)
- [x] Implementacja systemu kolorów dla każdego klocka
- [x] Implementacja losowania typu klocka
- [x] Implementacja sterowania (lewo, prawo, w dół, hard drop, soft drop)
- [x] Implementacja rotacji klocków (algorytm SRS)
- [x] Implementacja wall kick przy rotacji

---

- [x] Implementacja wykrywania pełnych linii
- [x] Implementacja usuwania linii i opadania bloków
- [x] Implementacja poziomów trudności
- [x] Implementacja przyspieszania gry z poziomem
- [x] Implementacja menu startowego
- [x] Dodanie czcionki do resources/

---

- [ ] Naprawić zapełnianie się planszy
- [x] Implementacja menu pauzy
- [x] Implementacja systemu punktacji (100/300/500/800 pkt)
- [x] Implementacja wyświetlania wyniku (prawy panel)
- [x] Implementacja tekstowego Game Over z wynikiem

---

- [x] Implementacja Ghost Piece (podgląd gdzie klocek spadnie)
- [x] Implementacja Hold Piece (przytrzymanie klocka)
- [x] Implementacja wyświetlania kolejki następnych klocków (3-5 klocków)
- [x] Implementacja systemu combo (mnożnik punktów za sekwencje)
- [x] Implementacja animacji usuwania linii

---

- [x] Implementacja lokalnej tablicy wyników (Top 10)
- [x] Implementacja statystyk gracza (czas gry, total linii, itp.)
- [ ] Implementacja konfiguracji sterowania (remapping klawiszy; w pliku)

---

- [x] Implementacja systemu dźwięków (SFX dla rotacji, drop, line clear)
- [x] Implementacja muzyki w tle
- [ ] Implementacja 2-3 motywów kolorystycznych
