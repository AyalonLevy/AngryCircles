#include "StructureBlock.h"

StructureBlock::StructureBlock(float x, float y, float width, float height, float mass, sf::Color color)
	: Entity(x, y, mass, color), m_maxHealth(100.0f), m_health(100.0f)
{
	m_shape.setSize(sf::Vector2f(width, height));
	m_shape.setFillColor(m_color);
	m_shape.setOrigin(width / 2.0f, height / 2.0f);
	m_shape.setPosition(m_position);
}

void StructureBlock::update(float dt) {
	// Blocks don't move by themselves, but gravity can
	// be added here later (for falling down after a hit)
	m_shape.setPosition(m_position);
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
