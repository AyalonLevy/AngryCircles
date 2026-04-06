#pragma once
#include <SFML/Graphics.hpp>

namespace Config {
	// Physics
	const float GRAVITY = 981.0f;  // [pixels/s^2]
	const float FRICTION = 0.98f;
	const float MIN_VELOCITY = 10.0f;  // [pixels/s] Stop moving if slower than this

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

	struct BlockProfile {
		float width;
		float height;
		float mass;
		sf::Color color;
	};

	const BlockProfile WOOD_BLOCK = { 40.0f, 60.0f, 2.0f, sf::Color(160, 82, 45) };

	struct LevelConfig {
		int rows;
		int cols;
		float spacing;
		sf::Vector2f startPos;
	};

	const LevelConfig LEVEL_1 = { 5, 3, 5.0f, {800.0f, 600.0f} };
}