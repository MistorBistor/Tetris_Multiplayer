cmake_minimum_required(VERSION 3.10)
project(Tetris VERSION 1.0.0 LANGUAGES CXX)

# Ustaw standard C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Znajdź SFML 2.5.1
find_package(SFML 2.5.1 COMPONENTS system window graphics audio REQUIRED)

# Ustaw katalogi z plikami nagłówkowymi
include_directories(include)

# Lista wszystkich plików źródłowych
set(SOURCES
    src/main.cpp
    src/GameEngine.cpp
    src/Board.cpp
    src/Tetromino.cpp
    src/Score.cpp
    src/Menu.cpp
    src/FileManager.cpp
)

# Utwórz plik wykonywalny
add_executable(tetris ${SOURCES})

# Linkuj z bibliotekami SFML
target_link_libraries(tetris sfml-system sfml-window sfml-graphics sfml-audio)

# Opcje kompilacji (ostrzeżenia)
if(MSVC)
    target_compile_options(tetris PRIVATE /W4)
else()
    target_compile_options(tetris PRIVATE -Wall -Wextra -Wpedantic)
endif()
