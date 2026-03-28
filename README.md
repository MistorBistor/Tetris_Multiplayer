# Tetris Multiplayer Documentation

## Technologies
- **Programming Language**: C++ 17
- **Graphics Library**: SFML 2.5.1
- **Build System**: CMake 3.10

## Installation Instructions
1. Clone the repository:
   ```bash
   git clone https://github.com/MistorBistor/Tetris_Multiplayer.git
   cd Tetris_Multiplayer
   ```
2. Install the required dependencies (SFML, etc.).
3. Create a build directory:
   ```bash
   mkdir build
   cd build
   ```
4. Run CMake to configure the project:
   ```bash
   cmake ..
   ```
5. Build the project:
   ```bash
   make
   ```

## Project Structure
- **src/**: Contains source files for game logic, modules, and functionality.
- **include/**: Header files for class declarations and function prototypes.
- **resources/**: Contains assets (images, sounds, etc.) used in the game.
- **CMakeLists.txt**: CMake build configuration file.

## Game Modules
- **GameLogic**: Handles game rules and mechanics.
- **UserInterface**: Manages user inputs and game states.
- **AssetManager**: Manages loading and unloading of resources.

## Authors
- **MistorBistor**: Main developer
- [Additional Contributors] (Add names if applicable)

## Planned Features
- Online multiplayer mode
- Local multiplayer support
- Leaderboards
- Customizable Tetris pieces
- Enhanced graphics and animations
- Sound effects and background music

---

This documentation outlines the essential components of the Tetris Multiplayer project. For further details, please refer to the code and additional resources provided in the repository.