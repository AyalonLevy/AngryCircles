#include "StructureBlock.h"

StructureBlock::StructureBlock(float x, float y, float width, float height, float mass, sf::Color color)
	: Entity(x, y, mass, color), m_maxHealth(100.0f), m_health(100.0f)
{
	m_shape.setSize(sf::Vector2f(width, height));
	m_shape.setFillColor(m_color);
	m_shape.setOrigin(width / 2.0f, height / 2.0f);
	m_shape.setPosition(m_position);

	// Inertia of a solid disk: I = 1/12 * m * (w^2 + h^2)`
	m_inertia = (1.0f / 12.0f) * m_mass * (width * width + height * height);
}

void StructureBlock::update(float dt) {
	if (m_isSleeping) return;

	// Apply gravity
	bool isOnFloor = (m_position.y + m_shape.getSize().y / 2.0f >= Config::FLOOR_Y);

	if (!isOnFloor) {
		m_velocity.y += Config::GRAVITY * dt;
	}

	// Apply Damping (Mechanical friction/Air resistance)
	if (m_velocity != sf::Vector2f(0, 0)) {
		float speed = std::sqrt(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y);

		if (speed < Config::SLEEP_THRESHOLD && std::abs(m_angularVelocity) < Config::MIN_ANGULAR_VELOCITY) {
			m_velocity = { 0.f, 0.f };
			m_angularVelocity = 0.f;
			setSleeping(true);
		}
	}

	float damping = std::pow(Config::FRICTION, dt * 60.0f);
	m_velocity *= damping;

	float angualrDamping = std::pow(Config::ANGULAR_DAMPING, dt * 60.0f);
	m_angularVelocity *= angualrDamping;

	// Update position and rotation
	m_position += m_velocity * dt;
	m_angle += m_angularVelocity * dt;

	// Micro-vibration threshold`
	if (std::abs(m_velocity.x) < Config::MIN_VELOCITY) m_velocity.x = 0.0f;
	if (std::abs(m_velocity.y) < Config::MIN_VELOCITY) m_velocity.y = 0.0f;
	if (std::abs(m_angularVelocity) < Config::MIN_ANGULAR_VELOCITY) m_angularVelocity = 0.0f;

	// Sync with SFML shape
	m_shape.setPosition(m_position);
	m_shape.setRotation(m_angle);

	// Floor constraint with rotation (Simple version for now)
	float halfHeight = m_shape.getSize().y / 2.0f;
	float penetration = (m_position.y + halfHeight) - Config::FLOOR_Y;

	if (penetration > 0.0f) {
		// Push object out of the floor
		m_position.y -= penetration;

		// Stop downward velocity only
		if (m_velocity.y > 0) m_velocity.y = 0;

		// Stabilize rotation slightly
		if (std::abs(m_angularVelocity) < Config::MIN_ANGULAR_VELOCITY) m_angularVelocity = 0;
	}
}

void StructureBlock::draw(sf::RenderWindow& window) {
	if (!isDestroyed()) {
		window.draw(m_shape);
	}
}

void StructureBlock::takeDamage(float damage) {
	m_health -= damage;
	if (m_health < 0) m_health = 0;

	// Darken the color as it takes damage
	sf::Color current = m_shape.getFillColor();
	m_shape.setFillColor(sf::Color(current.r * Config::DAMAGE_COLOR_MULTIPLIER, current.g * Config::DAMAGE_COLOR_MULTIPLIER, current.b * Config::DAMAGE_COLOR_MULTIPLIER));
}
