#pragma once
#include <vector>
#include <memory>
#include "Constants.h"
#include "Projectile.h"
#include "StructureBlock.h"

class Game {
private:
	sf::RenderWindow m_window;

	// Gameplay State
	int m_shotsRemaining;
	bool m_isAiming;

	// Vectors for Input
	sf::Vector2f m_mousePressPos;
	sf::Vector2f m_currentMousePos;

	// Entity Containers
	std::vector<std::unique_ptr<Projectile>> m_projectiles;
	std::vector<std::unique_ptr<StructureBlock>> m_blocks;

	// Initialization
	void initWindow();
	void spawnLevel();  // Helper to setup the blocks
	void initSlignshot();

	// Core Loop
	void processEvents();
	void update(float dt);
	void render();

	// Logic
	void fireProjectile(sf::Vector2f velocity);
	sf::Vector2f calculateLaunchVelocity();

public:
	Game();
	void run();
};