#include "StructureBlock.h"

StructureBlock::StructureBlock(float x, float y, float width, float height, float mass, float staticFriction, float dynamicFriction, float restitution, sf::Color color)
	: Entity(x, y, mass, staticFriction, dynamicFriction, restitution, color), m_maxHealth(100.0f), m_health(100.0f)
{
	m_shape.setSize(sf::Vector2f(width, height));
	m_shape.setFillColor(m_color);
	m_shape.setOrigin(width / 2.0f, height / 2.0f);

	// Inertia of a solid disk: I = 1/12 * m * (w^2 + h^2)`
	m_inertia = (1.0f / 12.0f) * m_mass * (width * width + height * height);
	m_invInertia = (m_inertia > 0) ? 1.0f / m_inertia : 0.0f;
}

void StructureBlock::update(float dt) {
	// Sync with SFML shape
	m_shape.setPosition(getPosition());
	m_shape.setRotation(getAngleDegrees());
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

std::vector<sf::Vector2f> StructureBlock::getVertices() const
{
	std::vector<sf::Vector2f> vertices;
	vertices.reserve(4);

	sf::Vector2f pos = getPosition();
	sf::Vector2f h = m_shape.getSize() * 0.5f;
	float rad = getRotation() * (Config::PI / 180.f);

	// Define 4 cornrs in Local Space (relative to centr at (0, 0))
	sf::Vector2f localCorners[4] = {
		{-h.x, -h.y},
		{ h.x, -h.y},
		{ h.x,  h.y},
		{-h.x,  h.y}
	};

	// Rotate and translate to World Space
	for (int i = 0; i < 4; i++) {
		float rx = localCorners[i].x * std::cos(rad) - localCorners[i].y * std::sin(rad);
		float ry = localCorners[i].x * std::sin(rad) + localCorners[i].y * std::cos(rad);
		vertices.push_back(sf::Vector2f(rx, ry) + pos);
	}

	return vertices;
}
