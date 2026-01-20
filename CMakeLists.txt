cmake_minimum_required(VERSION 3.10)
project(Tetris VERSION 1.0.0 LANGUAGES CXX)

# Export compile commands for tools like clangd, clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find SFML 2.5.1
find_package(SFML 2.5.1 COMPONENTS system window graphics audio REQUIRED)

# Collect all source files
file(GLOB_RECURSE SOURCES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.c"
)

# Collect all header files for clang-format
file(GLOB_RECURSE HEADERS
    "${CMAKE_SOURCE_DIR}/include/*.h"
    "${CMAKE_SOURCE_DIR}/include/*.h"
)

# Create executable
add_executable(tetris ${SOURCES} "include/TetrominoType.h")

# Use target_include_directories instead of include_directories (modern CMake)
target_include_directories(tetris PRIVATE include)

# Link SFML libraries
target_link_libraries(tetris PRIVATE
    sfml-system
    sfml-window
    sfml-graphics
    sfml-audio
)

# Compiler warnings
if(MSVC)
    target_compile_options(tetris PRIVATE /W4)
else()
    target_compile_options(tetris PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Clang-format target
find_program(CLANG_FORMAT_EXECUTABLE NAMES clang-format)

if(CLANG_FORMAT_EXECUTABLE)
    add_custom_target(
        clang-format
        COMMAND ${CLANG_FORMAT_EXECUTABLE}
        -i
        -style=file
        ${SOURCES} ${HEADERS}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running clang-format on source and header files"
        VERBATIM
    )

    # Optional: Add a target to check formatting without modifying files
    add_custom_target(
        check-format
        COMMAND ${CLANG_FORMAT_EXECUTABLE}
        --dry-run
        --Werror
        -style=file
        ${SOURCES} ${HEADERS}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Checking code formatting (no modifications)"
        VERBATIM
    )
else()
    message(WARNING "clang-format not found. Format targets will not be available.")
endif()

# Make tetris depend on clang-format (formats before every build)
if(CLANG_FORMAT_EXECUTABLE)
    add_dependencies(tetris clang-format)
endif()

# Optional: Install target (if you want to install the executable)
install(TARGETS tetris DESTINATION bin)
