#pragma once
#include "Entity.h"

class Projectile : public Entity {
private:
	sf::CircleShape m_shape;
	bool m_isLaunched = false;

public:
	// Pass everything needed to configure the projectile
	Projectile(float x, float y, float radius, float mass, float staticFriction, float dynamicFriction, float restitution, sf::Color color);

	void launch(sf::Vector2f veocity);
	void update(float dt) override;
	void draw(sf::RenderWindow& window) override;

	float getRadius() const { return m_shape.getRadius(); }
};
