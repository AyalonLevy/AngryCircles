#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"

class Entity {
protected:
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
	float m_angle = 0.0f;			// [deg]
	float m_angularVelocity = 0.0f;	// [deg/s]
	float m_mass;
	float m_inertia = 0.0f;

	bool m_isSleeping = false;

	sf::Color m_color;

public:
	// Constructor using member initializer list (Modern C++ Best Practice)
	Entity(float x, float y, float mass, sf::Color color)
		: m_position(x, y), m_velocity(0.f, 0.f), m_mass(mass), m_color(color) {}

	// Virtual destructor is REQUIRED when using inheritance to prevent memory leaks
	virtual ~Entity() = default;

	// Getters / Setters
	sf::Vector2f getPosition() const { return m_position; }
	void setPosition(sf::Vector2f pos) { m_position = pos; }

	sf::Vector2f getVelocity() const { return m_velocity; }
	void setVelocity(sf::Vector2f vel) { m_velocity = vel; }

	float getAngularVelocity() const { return m_angularVelocity; }
	void setAngularVelocity(float angle) { m_angularVelocity = angle; }

	float getMass() const { return m_mass; }
	float getInertia() const { return m_inertia; }

	bool getSleeping() const { return m_isSleeping; }
	void setSleeping(bool state) { m_isSleeping = state; }

	// Core Interfacae
	virtual void update(float dt) = 0;
	virtual void draw(sf::RenderWindow& window) = 0;
};