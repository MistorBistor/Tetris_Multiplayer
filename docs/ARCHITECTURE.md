# Dokumentacja Tetris

## Architektura

Projekt Tetris jest zorganizowany w modułowej architekturze, gdzie każdy moduł odpowiada za konkretną funkcjonalność gry.

### Moduły

#### 1. GameEngine
Centralny punkt gry odpowiedzialny za:
- Główną pętlę gry
- Zarządzanie stanem gry (menu, rozgrywka, pauza, game over)
- Koordynację wszystkich innych modułów
- Obsługę zdarzeń systemowych

#### 2. Board
Zarządza planszą gry:
- Siatka 10x20 komórek
- Wykrywanie kolizji klocków z planszą i granicami
- Sprawdzanie i usuwanie zapełnionych linii
- Renderowanie planszy

#### 3. Tetromino
Reprezentuje klocki Tetris:
- 7 typów klocków: I, O, T, S, Z, J, L
- Rotacja klocków (4 stany rotacji)
- Przemieszczanie (lewo, prawo, dół)
- Renderowanie klocka

#### 4. Score
System punktacji:
- Zliczanie punktów za usunięte linie
- Zarządzanie poziomami trudności
- Przechowywanie i wyświetlanie rekordów
- Obliczanie prędkości spadania klocków

#### 5. Menu
Interfejs użytkownika:
- Menu główne
- Menu pauzy
- Ekran końca gry
- Nawigacja klawiaturą

#### 6. FileManager
Zarządzanie danymi:
- Zapis/odczyt stanu gry
- Zapis/odczyt najwyższych wyników
- Zapis/odczyt ustawień

## System punktacji

Punkty są przyznawane za usuwanie linii zgodnie z zasadami klasycznego Tetrisa:

- **1 linia**: 40 × (poziom + 1) punktów
- **2 linie**: 100 × (poziom + 1) punktów
- **3 linie**: 300 × (poziom + 1) punktów
- **4 linie (Tetris)**: 1200 × (poziom + 1) punktów

Poziom trudności zwiększa się co 10 usuniętych linii, co powoduje przyśpieszenie spadania klocków.

## Sterowanie

### W grze
- **Strzałka w lewo**: Przesuń klocek w lewo
- **Strzałka w prawo**: Przesuń klocek w prawo
- **Strzałka w dół**: Przyśpiesz opadanie
- **Strzałka w górę / Z**: Obróć klocek w lewo
- **X**: Obróć klocek w prawo
- **Spacja**: Upuść klocek na dół
- **P / Escape**: Pauza
- **Q**: Wyjście do menu

### W menu
- **Strzałka w górę/dół**: Nawigacja
- **Enter**: Wybór opcji
- **Escape**: Powrót/Wyjście

## Formaty plików

### Zapis gry (save.dat)
Plik zawiera:
- Aktualny wynik
- Poziom
- Liczba usuniętych linii
- Stan planszy
- Aktualny klocek

### Najwyższy wynik (highscore.dat)
Pojedyncza liczba całkowita reprezentująca najwyższy wynik.

### Ustawienia (settings.dat)
Plik tekstowy w formacie klucz=wartość:
```
volume=70
music_enabled=1
sound_effects_enabled=1
```

## Rozwój projektu

### Kolejne kroki implementacji

1. **Faza 1**: Podstawowa funkcjonalność
   - Implementacja klocków i planszy
   - Podstawowa mechanika gry (ruch, rotacja, kolizje)
   - System punktacji

2. **Faza 2**: Interfejs użytkownika
   - Menu główne
   - Wyświetlanie wyniku i statystyk
   - Ekran końca gry

3. **Faza 3**: Funkcje zaawansowane
   - Zapis i wczytywanie gry
   - Efekty dźwiękowe i muzyka
   - Animacje

4. **Faza 4**: Polishing
   - Balansowanie trudności
   - Optymalizacja wydajności
   - Testy i debugowanie

## Wymagania techniczne

### Minimalne wymagania sprzętowe
- Procesor: 1 GHz
- RAM: 512 MB
- Karta graficzna: Obsługa OpenGL 1.1
- System operacyjny: Windows 7+, Linux (kernel 2.6+), macOS 10.7+

### Zależności bibliotek
- SFML 2.5.1 (system, window, graphics, audio)
- Standardowa biblioteka C++17

## Licencja

Projekt jest dostępny na licencji MIT. Zobacz plik LICENSE dla szczegółów.
