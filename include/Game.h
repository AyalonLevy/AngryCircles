//#pragma once
//#include <vector>
//#include <memory>
//#include "Constants.h"
//#include "Projectile.h"
//#include "StructureBlock.h"
//
//class Game {
//private:
//	sf::RenderWindow m_window;
//
//	// Gameplay State
//	int m_shotsRemaining;
//	bool m_isAiming;
//
//	// Vectors for Input
//	sf::Vector2f m_mousePressPos;
//	sf::Vector2f m_currentMousePos;
//
//	// Entity Containers
//	std::vector<std::unique_ptr<Projectile>> m_projectiles;
//	std::vector<std::unique_ptr<StructureBlock>> m_blocks;
//
//	// The Foloor Entity
//	std::unique_ptr<StructureBlock> m_floor;
//
//	// Initialization
//	void initWindow();
//	void spawnLevel();  // Helper to setup the blocks
//	void initSlignshot();
//
//	// Core Loop
//	void processEvents();
//	void update(float dt);
//	void render();
//
//	// Logic
//	void fireProjectile(sf::Vector2f velocity);
//	sf::Vector2f calculateLaunchVelocity();
//
//public:
//	Game();
//	void run();
//};


#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "PhysicsWorld.h"
#include "Shapes.h"

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();

    // SFML Core
    sf::RenderWindow m_window;

    // Physics Core
    PhysicsWorld m_world;
    std::unique_ptr<RigidBody> m_floor;
    std::vector<std::unique_ptr<RigidBody>> m_bodies;

    // Benchmarking
    void printDiagnostics();
};