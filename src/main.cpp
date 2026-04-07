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



// TODO:
// 1. Add trajectory line (dashed - for the begining of the trajectory
// 2. Allow Rotation
// 3. Add Coliision
// 4. Add "Projectile loading
// 5. Add score
// 6. Create nice structure for dropping (small tower)
// 7. Add to the level constructor (init) the projectiles available for this level
// 8. Create a structure generator
// 9. Create a level geneator - to make it infinite?
// 