#pragma once
#include <SFML/Graphics.hpp>

namespace Config {
	// Physics
	const float GRAVITY = 981.0f;  // [px/s^2]
	const float PI = 3.14159265f;

	struct Material {
		float staticFriction;  // mu_s
		float kineticFriction; // mu_k
		float restitution;     // bounciness (e)
	};

	// Example: Wood
	const Material WOOD = { 0.5f, 0.35f, 0.2f };
	const Material STEEL = { 0.4f, 0.3f, 0.1f };
	const Material RUBBER = { 0.9f, 0.8f, 0.8f };


	const float FRICTION = 0.999f;			// Linear damping (by 2%)
	const float ANGULAR_DAMPING = 0.95f;	// Reduce rotation by 5% per frame
	const float MAX_LINEAR_VELOCITY = 2000.0f;	// [px/s]
	const float MIN_LINEAR_VELOCITY = 0.5f;		// [px/s] Stop moving if slower than this
	const float MIN_TANGENT_VELOCITY = 0.01f;	// [px/s]
	const float MIN_ANGULAR_VELOCITY = 0.01f;	// [deg/s] Stop rotating if slower than this
	const float MAX_ANGULAR_VELOCITY = 50.0f;	// [deg/s]
	const float FLOOR_Y = 650.0f;
	const float RESTITUTION_VELOCITY_THRESHOLD = 1.0f;
	const float PENETRATION_CORRETION_PERCENTAGE = 0.2f;
	const float SLEEP_THRESHOLD = 0.2f;		// Stop miving if speed < 1[px/s]
	const float SLOP = 0.05f;				// Allow 0.02px overlap before resolving
	const float EPSILON = 0.0001f;			// 1e-4

	const int ITERATIONS = 8;  // Number of physics iterations per collision pass


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
		float staticFriction;
		float dynamicFriction;
		float restitution;
		sf::Color color;
	};

	const ProjectileProfile STANDARD_BALL = {
		15.0f,
		1.0f,
		0.4f,
		0.3f,
		0.6f,   // nice bounce
		sf::Color::Red
	};

	const ProjectileProfile HEAVY_BALL = {
		25.0f,
		3.0f,
		0.5f,
		0.3f,
		0.2f,
		sf::Color::Blue
	};
	
	// Block Profiles
	struct BlockProfile {
		float width;
		float height;
		float mass;
		float staticFriction;
		float dynamicFriction;
		float restitution;
		sf::Color color;
	};

	const BlockProfile WOOD_BLOCK = {
		10.0f,
		60.0f,
		2.0f,
		0.6f,
		0.4f,
		0.2f,   // wood barely bounces
		sf::Color(160, 82, 45)
	};

	// Level Profiles
	struct LevelConfig {
		int rows;
		int cols;
		float spacing;
		sf::Vector2f startPos;
	};

	const LevelConfig LEVEL_1 = { 1, 1, 20.0f, {800.0f, 600.0f} };
}