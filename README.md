# Tetris Multiplayer

## Spis treści  
1. [Wprowadzenie](#wprowadzenie)  
2. [Jak zainstalować](#jak-zainstalować)  
3. [Struktura projektu](#struktura-projektu)  
4. [Moduły gry](#moduły-gry)  
5. [Autorzy](#autorzy)  
6. [TODO](#todo)  

## Wprowadzenie  
Tetris Multiplayer to gra wieloosobowa w Tetrisa, która umożliwia rozgrywkę na różnych platformach.

## Jak zainstalować  
### macOS  
1. Upewnij się, że masz zainstalowanego Pythona 3.6 lub nowszego.  
2. Zainstaluj zależności: `pip install -r requirements.txt`  
3. Uruchom grę: `python main.py`  

### Windows  
1. Upewnij się, że masz zainstalowanego Pythona 3.6 lub nowszego.  
2. Zainstaluj zależności: `pip install -r requirements.txt`  
3. Uruchom grę: `python main.py`  

## Struktura projektu  
```
Tetris_Multiplayer/  
│   README.md  
│   main.py  
│   requirements.txt  
│
├── game/  
│   ├── __init__.py  
│   ├── game.py  
│   ├── player.py  
│   └── board.py  
│
└── assets/  
    ├── images/  
    ├── sounds/  
    └── fonts/  
```  

## Moduły gry  
- `game.py`: zawiera logikę gry.  
- `player.py`: zarządza stanem gracza.  
- `board.py`: odpowiada za wyświetlanie planszy.  

## Autorzy  
- MistorBistor  
- Inny współautor  

## TODO  
- Dodaj więcej poziomów trudności.  
- Optymalizuj kod.  
- Dodaj więcej modułów do gry.