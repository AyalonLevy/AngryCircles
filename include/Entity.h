#pragma once
#include <SFML/Graphics.hpp>
#include "Constants.h"

class Entity {
protected:
	// Linear
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
	sf::Vector2f m_force;	// Reset to 0 every frame after integration

	// Angular
	float m_angle = 0.0f;			// [rad] for math
	float m_angularVelocity = 0.0f;	// [rad/s]
	float m_torque;					// Reset to 0 every frame

	// Constants
	float m_mass;
	float m_invMass;
	float m_inertia;
	float m_invInertia;
	float m_staticFriction = 0.6f;
	float m_dynamicFriction = 0.4f;
	float m_restitution = 0.5f;

	bool m_isSleeping;
	sf::Color m_color;

public:
	void applyForce(sf::Vector2f force, sf::Vector2f contactPoint) {
		m_force += force;

		// Torque = r x F
		sf::Vector2f r = contactPoint - m_position;
		m_torque += (r.x * force.y - r.y * force.x);
	}

	void integrate(float dt) {
		if (m_invMass == 0)
			return;

		// Update Velocities
		sf::Vector2f acceleration = m_force * m_invMass;
		m_velocity += acceleration * dt;
		float speed = std::sqrt(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y);
		
		// Linear clamp
		if (speed > Config::MAX_LINEAR_VELOCITY)
			m_velocity = (m_velocity / speed) * Config::MAX_LINEAR_VELOCITY;
		
		// Angular clamp
		if (std::abs(m_angularVelocity) > Config::MAX_ANGULAR_VELOCITY)
			m_angularVelocity = (m_angularVelocity > 0 ? 1 : -1) * Config::MAX_ANGULAR_VELOCITY;


		float angularAccel = m_torque * m_invInertia;
		m_angularVelocity += angularAccel * dt;

		// Apply Damping
		m_velocity *= Config::FRICTION;
		m_angularVelocity *= Config::ANGULAR_DAMPING;

		// Update Positions
		m_position += m_velocity * dt;
		m_angle += m_angularVelocity * dt;
		
		// Sleep logic (stopping micro-vinrations)
		if (std::abs(m_velocity.x) < Config::MIN_LINEAR_VELOCITY) m_velocity.x = 0;
		if (std::abs(m_velocity.y) < Config::MIN_LINEAR_VELOCITY) m_velocity.y = 0;
		if (std::abs(m_angularVelocity) < Config::MIN_ANGULAR_VELOCITY) m_angularVelocity = 0;

		// Reset accumulators
		m_force = { 0.f, 0.f };
		m_torque = 0.f;
	}

	// Constructor using member initializer list (Modern C++ Best Practice)
	Entity(float x, float y, float mass, float staticFriction,
		float dynamicFriction, float restitution, sf::Color color)
		: m_position(x, y), m_velocity(0.f, 0.f), m_force(0.f, 0.f),
		m_angle(0.f), m_angularVelocity(0.f), m_torque(0.f),
		m_mass(mass), m_staticFriction(staticFriction),
		m_dynamicFriction(dynamicFriction), m_restitution(restitution),
		m_isSleeping(false), m_color(color)
	{
		// If mass is 0, we treat it as infinite (static)
		if (m_mass == 0.f) {
			m_invMass = 0.f;
			m_invInertia = 0.f;
		}
		else {
			m_invMass = 1.0f / m_mass;
			// m_invInertia will be set by the child class based on shape
		}
	}

	// Virtual destructor is REQUIRED when using inheritance to prevent memory leaks
	virtual ~Entity() = default;

	// Getters / Setters
	sf::Vector2f getPosition() const { return m_position; }
	void setPosition(sf::Vector2f pos) { m_position = pos; }

	sf::Vector2f getVelocity() const { return m_velocity; }
	void setVelocity(sf::Vector2f vel) { m_velocity = vel; }

	float getAngularVelocity() const { return m_angularVelocity; }
	void setAngularVelocity(float angle) { m_angularVelocity = angle; }

	bool getSleeping() const { return m_isSleeping; }
	void setSleeping(bool state) { m_isSleeping = state; }

	float getAngle() const { return m_angle; }	// [rad]
	float getAngleDegrees() const { return m_angle * (180.0f / Config::PI); }

	float getMass() const { return m_mass; }
	float getInvMass() const { return m_invMass; }

	float getInertia() const { return m_inertia; }
	float getInvInertia() const { return m_invInertia; }

	float getStaticFriction() const { return m_staticFriction; }
	float getDynamicFriction() const { return m_dynamicFriction; }
	float getRestitution() const { return m_restitution; }

	// Core Interfacae
	virtual void update(float dt) = 0;
	virtual void draw(sf::RenderWindow& window) = 0;
};