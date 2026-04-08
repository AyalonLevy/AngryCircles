#include "Projectile.h"

Projectile::Projectile(float x, float y, float radius, float mass, float staticFriction, float dynamicFriction, float restitution, sf::Color color)
	: Entity(x, y, mass, staticFriction, dynamicFriction, restitution, color)  // Call parent constructor
{
	m_shape.setRadius(radius);
	m_shape.setFillColor(m_color);
	m_shape.setOrigin(radius, radius);	// Center of Mass at center of circle

	// Inertia of a solid disk: I = 1/2 * m * r^2
	m_inertia = 0.5f * m_mass * (radius * radius);
	m_invInertia = (m_inertia > 0) ? 1.0f / m_inertia : 0.0f;
}

void Projectile::launch(sf::Vector2f initialVelocity) {
	m_velocity = initialVelocity;
	m_isLaunched = true;
}

void Projectile::update(float dt) {
	m_shape.setPosition(getPosition());
	m_shape.setRotation(getAngleDegrees());
}

void Projectile::draw(sf::RenderWindow& window) {
	window.draw(m_shape);
}