#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"

class Entity {
protected:
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
	float m_mass;
	sf::Color m_color;

public:
	// Constructor using member initializer list (Modern C++ Best Practice)
	Entity(float x, float y, float mass, sf::Color color)
		: m_position(x, y), m_velocity(0.f, 0.f), m_mass(mass), m_color(color) {}

	// Virtual destructor is REQUIRED when using inheritance to prevent memory leaks
	virtual ~Entity() = default;

	// Pure virtual functions (must be implemented by children)
	virtual void update(float dt) = 0;
	virtual void draw(sf::RenderWindow& window) = 0;

	// Getters (to avoid magic numbers elsewhere)
	sf::Vector2f getPosition() const { return m_position; }
	float getMass() const { return m_mass; }
};