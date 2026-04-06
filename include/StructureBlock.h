#pragma once
#include "Entity.h"

class StructureBlock : public Entity {
private:
	sf::RectangleShape m_shape;
	float m_health = 100.0f;
	float m_maxHealth;

public:
	StructureBlock(float x, float y, float width, float height, float mass, sf::Color color);

	void update(float dt) override;
	void draw(sf::RenderWindow& window) override;

	// Handling impacts
	void takeDamage(float amount);
	bool isDestroyed() const { return m_health <= 0; }
	float getDamagePercentage() const { return (1.0f - (m_health / m_maxHealth)) * 100.0f; }
};
