#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"

class RigidBody {
public:
	int id;
	std::string name;
	static int nextID;

	// State
	sf::Vector2f position;
	sf::Vector2f velocity;
	float rotation = 0.0f;			// [rad]
	float angularVelocity = 0.0f;	// [rad/s]

	// Properties
	float mass;
	float invMass;					// Pre-calculated 1/m for speed
	float inertia;
	float invInertia;				// Pre-calculates 1/I
	Config::Material material;

	// Accumulators
	sf::Vector2f forceAccumulator;
	float torqueAccumulator = 0.0f;
	
	RigidBody(float m, Config::Material mat, std::string n);

	virtual ~RigidBody() = default;

	void applyForce(sf::Vector2f force, sf::Vector2f worldContactPoint);

	void integrate(float dt);
	virtual void draw(sf::RenderWindow& window) = 0;
};