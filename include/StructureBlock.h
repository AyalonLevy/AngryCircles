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

	// Returns the corners of the rectangle in world space
	std::vector<sf::Vector2f> getVertices() const;

	// Returns ther normalized local X and Y areas of the shape
	sf::Vector2f getLocalXAxis() const;
	sf::Vector2f getLocalYAxis() const;

	sf::Vector2f getSize() const { return m_shape.getSize(); }
	float getRotation() const { return m_shape.getRotation(); }  // in degrees
};
