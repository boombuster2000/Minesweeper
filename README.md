# Minesweeper

A simple Minesweeper game built in C++.

## Controls
- **Left Click** - Uncover a tile
- **Right Click** - Place or remove a flag
- **ESC** - Exit the game

## Building the Project
This project uses CMake for building and is dependent on the Raylib library, which CMake downloads automatically. To build the project, follow these steps:

1. Navigate to the project directory:
   ```sh
   cd /path/to/minesweeper
   ```
2. Create a build directory:
   ```sh
   mkdir build && cd build
   ```
3. Run CMake to generate the build files:
   ```sh
   cmake ..
   ```
4. Compile the project:
   ```sh
   cmake --build .
   ```

## Requirements
- C++ compiler (GCC, Clang, or MSVC)
- CMake

## Running the Game
After building, the executable can be found in the build directory. Run it with:
   ```sh
   ./minesweeper
   ```

Enjoy the game!

