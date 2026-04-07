#include "Projectile.h"

Projectile::Projectile(float x, float y, float radius, float mass, sf::Color color)
	: Entity(x, y, mass, color)  // Call parent constructor
{
	m_shape.setRadius(radius);
	m_shape.setFillColor(m_color);
	m_shape.setOrigin(radius, radius);
	m_shape.setPosition(m_position);

	// Inertia of a solid disk: I = 1/2 * m * r^2
	m_inertia = 0.5f * m_mass * (radius * radius);
}

void Projectile::launch(sf::Vector2f initialVelocity) {
	m_velocity = initialVelocity;
	m_isLaunched = true;
}

void Projectile::update(float dt) {
	if (!m_isLaunched) return;

	// Apply Gravity (Acceleration)
	const float GRAVITY = Config::GRAVITY;  // [pixels/s^2]

	// Kinematics: V = V0 + at
	m_velocity.y += GRAVITY * dt;

	// Kinematics: X = X0 + vt
	m_position += m_velocity * dt;

	// Floor Collision Constraint
	if (m_position.y + m_shape.getRadius() > Config::FLOOR_Y) {
		m_position.y = Config::FLOOR_Y - m_shape.getRadius();	// Snap to top of floor
		m_velocity.y *= -Config::BOUNCE_RESTITUTION;			// Reverse and dampen Y velocity
		m_velocity.x *= Config::FRICTION;						// Apply ground friction
	}

	m_shape.setPosition(m_position);
}

void Projectile::draw(sf::RenderWindow& window) {
	window.draw(m_shape);
}