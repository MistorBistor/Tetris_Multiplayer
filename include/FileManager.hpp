#ifndef FILE_MANAGER_HPP
#define FILE_MANAGER_HPP

#include <string>
#include <vector>

/**
 * @struct GameSave
 * @brief Struktura zapisu gry
 */
struct GameSave {
    int score;
    int level;
    int linesCleared;
    std::vector<std::vector<int>> boardState;
    // TODO: Dodać więcej pól jeśli potrzebne (aktualny tetromino, kolejny tetromino)
};

/**
 * @class FileManager
 * @brief Zarządzanie zapisem i odczytem plików
 * 
 * Klasa odpowiedzialna za zapisywanie i wczytywanie stanu gry,
 * najwyższych wyników i ustawień.
 */
class FileManager {
private:
    static const std::string SAVE_FILE;
    static const std::string HIGHSCORE_FILE;
    static const std::string SETTINGS_FILE;
    
public:
    /**
     * @brief Konstruktor
     */
    FileManager();
    
    /**
     * @brief Destruktor
     */
    ~FileManager();
    
    /**
     * @brief Zapis stanu gry do pliku
     * @param save Struktura z danymi do zapisu
     * @return true jeśli zapis się powiódł
     * TODO: Serializować dane gry do pliku (format tekstowy lub binarny)
     */
    bool saveGame(const GameSave& save) const;
    
    /**
     * @brief Wczytanie stanu gry z pliku
     * @param save Struktura do wypełnienia danymi
     * @return true jeśli wczytanie się powiodło
     * TODO: Deserializować dane z pliku
     */
    bool loadGame(GameSave& save) const;
    
    /**
     * @brief Sprawdzenie czy istnieje zapis gry
     * @return true jeśli plik zapisu istnieje
     * TODO: Sprawdzić czy plik SAVE_FILE istnieje
     */
    bool hasSavedGame() const;
    
    /**
     * @brief Usunięcie zapisu gry
     * TODO: Usunąć plik zapisu
     */
    void deleteSave() const;
    
    /**
     * @brief Zapis najwyższego wyniku
     * @param highScore Wartość najwyższego wyniku
     * TODO: Zapisać highScore do pliku
     */
    void saveHighScore(int highScore) const;
    
    /**
     * @brief Wczytanie najwyższego wyniku
     * @return Najwyższy wynik lub 0 jeśli brak pliku
     * TODO: Wczytać highScore z pliku
     */
    int loadHighScore() const;
    
    /**
     * @brief Zapis ustawień gry
     * @param settings Mapa ustawień (klucz-wartość)
     * TODO: Zapisać ustawienia do pliku (np. głośność, sterowanie)
     */
    void saveSettings(const std::vector<std::pair<std::string, std::string>>& settings) const;
    
    /**
     * @brief Wczytanie ustawień gry
     * @return Mapa ustawień
     * TODO: Wczytać ustawienia z pliku
     */
    std::vector<std::pair<std::string, std::string>> loadSettings() const;
};

#endif // FILE_MANAGER_HPP
