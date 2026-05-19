#pragma once
// Enum definiuj¹cy wszystkie mo¿liwe typy klocków w Tetrisie
enum class TetrominoType {
	I = 0,     // Cyan - linia
	O = 1,     // Yellow - kwadrat
	T = 2,     // Magenta - T-shape
	S = 3,     // Green - S-shape
	Z = 4,     // Red - Z-shape
	J = 5,     // Blue - J-shape
	L = 6,     // Orange - L-shape
	P = 7,  // Szary - penalty line
	Empty = -1 // Pusta komórka na planszy
};