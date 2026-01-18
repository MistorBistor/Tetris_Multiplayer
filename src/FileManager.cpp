#include "FileManager.hpp"
#include <fstream>

const std::string FileManager::SAVE_FILE = "save.dat";
const std::string FileManager::HIGHSCORE_FILE = "highscore.dat";
const std::string FileManager::SETTINGS_FILE = "settings.dat";

FileManager::FileManager() {
    // TODO: Inicjalizacja jeśli potrzebna
}

FileManager::~FileManager() {
    // TODO: Cleanup jeśli potrzebne
}

bool FileManager::saveGame(const GameSave& save) const {
    // TODO: Otworzyć plik SAVE_FILE do zapisu
    // TODO: Zapisać wszystkie pola struktury GameSave
    // TODO: Zwrócić true jeśli sukces, false przy błędzie
    return false;
}

bool FileManager::loadGame(GameSave& save) const {
    // TODO: Otworzyć plik SAVE_FILE do odczytu
    // TODO: Wczytać wszystkie pola do struktury GameSave
    // TODO: Zwrócić true jeśli sukces, false przy błędzie
    return false;
}

bool FileManager::hasSavedGame() const {
    // TODO: Sprawdzić czy plik SAVE_FILE istnieje
    // TODO: Można użyć std::ifstream lub std::filesystem
    return false;
}

void FileManager::deleteSave() const {
    // TODO: Usunąć plik SAVE_FILE
    // TODO: Można użyć std::remove lub std::filesystem::remove
}

void FileManager::saveHighScore(int highScore) const {
    // TODO: Otworzyć plik HIGHSCORE_FILE do zapisu
    // TODO: Zapisać wartość highScore
    std::ofstream file(HIGHSCORE_FILE);
    if (file.is_open()) {
        file << highScore;
        file.close();
    }
}

int FileManager::loadHighScore() const {
    // TODO: Otworzyć plik HIGHSCORE_FILE do odczytu
    // TODO: Wczytać i zwrócić wartość highScore
    // TODO: Jeśli plik nie istnieje, zwrócić 0
    std::ifstream file(HIGHSCORE_FILE);
    int score = 0;
    if (file.is_open()) {
        file >> score;
        file.close();
    }
    return score;
}

void FileManager::saveSettings(const std::vector<std::pair<std::string, std::string>>& settings) const {
    // TODO: Otworzyć plik SETTINGS_FILE do zapisu
    // TODO: Zapisać każdą parę klucz-wartość
    // TODO: Format: klucz=wartość w każdej linii
}

std::vector<std::pair<std::string, std::string>> FileManager::loadSettings() const {
    // TODO: Otworzyć plik SETTINGS_FILE do odczytu
    // TODO: Wczytać wszystkie pary klucz-wartość
    // TODO: Zwrócić wektor par
    return std::vector<std::pair<std::string, std::string>>();
}
