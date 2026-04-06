#include "Game.h"

int main() {
    // Instantiate the game engine
    // The constructor will handle initWindow()a and spawnLevel()
    Game game;

    // Start the game loop
    // This will run until the window is closed
    game.run();

    return 0;
}