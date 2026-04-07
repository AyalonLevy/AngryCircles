#pragma once
#include <vector>
#include <memory>
#include "Projectile.h"
#include "StructureBlock.h"

class CollisionManager {
public:
	static void handleCollisions(
		std::vector<std::unique_ptr<Projectile>>& projectiles,
		std::vector<std::unique_ptr<StructureBlock>>& blocks
	);

private:
	// Circle vs Oriented Rectangle
	static bool checkCircleOBB(Projectile& circle, StructureBlock& block);

	// Rectangle vs Rectangle (OBB vs OBB)
	static bool checkOBBOBB(StructureBlock& a, StructureBlock& b);

	// The "Engine" part: pushing objects apart so the don't overlap
	static void resolveCollision(Entity& a, Entity& b, sf::Vector2f normal, float overlap, sf::Vector2f contactPoint);

	static sf::Vector2f rotateVector(sf::Vector2f vector, float angleRad) {
		return sf::Vector2f(
			vector.x * std::cos(angleRad) - vector.y * std::sin(angleRad),
			vector.x * std::sin(angleRad) + vector.y * std::cos(angleRad)
		);
	}

	static sf::Vector2f inverseRotateVector(sf::Vector2f vector, float angleRad) {
		return rotateVector(vector, -angleRad);
	}

	static void projectRectangle(const StructureBlock& rect, sf::Vector2f axis, float& min, float& max);
};