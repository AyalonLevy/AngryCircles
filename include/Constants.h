#pragma once
#include <SFML/Graphics.hpp>

namespace Config {
	// Physics
	const float GRAVITY = 981.0f;  // [px/s^2]
	const float FRICTION = 0.98f;
	const float MIN_VELOCITY = 1.f;  // [px/s] Stop moving if slower than this
	const float MIN_ANGULAR_VELOCITY = 0.1f;  // [deg/s] Stop rotating if slower than this
	const float FLOOR_Y = 650.0f;
	const float BOUNCE_RESTITUTION = 0.5f;  // Energy kept after a bounce (0 to 1)
	const float PI = 3.14159265f;
	const float PENETRATION_CORRETION_PERCENTAGE = 1.0f;
	const float ANGULAR_DAMPING = 0.95f;	// Reduce rotation by 5% per frame
	const float SLEEP_THRESHOLD = 1.0f;		// Stop miving if speed < 1[px/s]
	const float SLOP = 0.01f;				// Allow 0.02px overlap before resolving

	const int INERATIONS = 6;  // Number of physics iterations per collision pass

	// Combat / Gameplay
	const float DAMAGE_COLOR_MULTIPLIER = 0.8f;
	const float MAX_LAUNCH_POWER = 600.0f;
	const float SLINGSHOT_SENSITIVITY = 2.0f;  // Multiplier for drag distance

	// Window
	const unsigned int SCREEN_WIDTH = 1280;
	const unsigned int SCREEN_HEIGHT = 720;

	// Color (Data-Driven Defaults)
	const sf::Color COLOR_PROJECTILE = sf::Color::Red;
	const sf::Color COLOR_BLOCK = sf::Color(100, 100, 100);  // Gray

	// Slingshot Position
	const sf::Vector2f SLINGSHOT_POS = { 150.0f, 500.0f };

	// Projectile Profiles
	struct ProjectileProfile {
		float radius;
		float mass;
		sf::Color color;
	};

	const ProjectileProfile STANDARD_BALL = { 15.0f, 1.0f, sf::Color::Red };
	const ProjectileProfile HEAVY_BALL = { 25.0f, 3.0f, sf::Color::Blue };
	
	// Block Profiles
	struct BlockProfile {
		float width;
		float height;
		float mass;
		sf::Color color;
	};

	const BlockProfile WOOD_BLOCK = { 10.0f, 60.0f, 2.0f, sf::Color(160, 82, 45) };

	// Level Profiles
	struct LevelConfig {
		int rows;
		int cols;
		float spacing;
		sf::Vector2f startPos;
	};

	const LevelConfig LEVEL_1 = { 5, 3, 100.0f, {400.0f, 600.0f} };
}