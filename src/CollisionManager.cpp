#include "CollisionManager.h"
#include <iostream>

void CollisionManager::handleCollisions(std::vector<std::unique_ptr<Projectile>>& projectiles, std::vector<std::unique_ptr<StructureBlock>>& blocks, StructureBlock* floor) {
	for (int k = 0; k < Config::ITERATIONS; k++) {
		// All objects vs Floor
		if (floor) {
			for (auto& p : projectiles) checkCircleOBB(*p, *floor);
			for (auto& b : blocks) checkOBBOBB(*b, *floor);
		}

		// Projectiles vs blocks
		for (auto& p : projectiles) {
			for (auto& b : blocks) {
				checkCircleOBB(*p, *b);
			}
		}

		// Blocks vs Blocks
		for (size_t i = 0; i < blocks.size(); ++i) {
			for (size_t j = i + 1; j < blocks.size(); ++j) {
				checkOBBOBB(*blocks[i], *blocks[j]);
			}
		}
	}
}

bool CollisionManager::checkCircleOBB(Projectile& circle, StructureBlock& block)
{
	// 1. Extract Physics Data
	sf::Vector2f center = circle.getPosition();
	sf::Vector2f rectPos = block.getPosition();
	sf::Vector2f size = block.getSize();

	// Convert degrees to radians: rad = deg * (PI / 180)
	float angleRad = block.getRotation() * (Config::PI / 180.0f);  // [rad]

	// 2. Local Space Transformation
	// Formula: Translate world point to relatice, then apply Inverse Rotation Matrix
	sf::Vector2f relativePos = center - rectPos;

	sf::Vector2f localVector = inverseRotateVector(relativePos, angleRad);

	// 3.: Clamping (Finding Closest Point on AABB)
	// Find the point on the rectangle closest to the circle center in local space
	float halfW = size.x / 2.0f;
	float halfH = size.y / 2.0f;

	float closestX = std::max(-halfW, std::min(localVector.x, halfW));
	float closestY = std::max(-halfH, std::min(localVector.y, halfH));

	sf::Vector2f localContactPoint(closestX, closestY);

	// 4. Distance Check
	// Formula: Pythagorean Theorem in local space
	float dx = localVector.x - closestX;
	float dy = localVector.y - closestY;
	float distanceSquared = (dx * dx) + (dy * dy);
	float radius = circle.getRadius();

	if (distanceSquared < (radius * radius)) {
		float dist = std::sqrt(distanceSquared);

		// 5. Normal Reconstruction
		// Calculate normal in local space, then rotate back to World Space
		sf::Vector2f localNormal = (dist != 0) ? sf::Vector2f(dx / dist, dy / dist) : sf::Vector2f(0.0f, -1.0f);

		// Rotate Local Normal -> World Normal using Rotation Matrix
		sf::Vector2f worldNormal = rotateVector(localNormal, angleRad);

		// Rotate local contact point back to world space and add the rectangle's world position
		sf::Vector2f worldContactPoint = rotateVector(localContactPoint, angleRad) + rectPos;

		float overlap = radius - dist;
		resolveCollision(circle, block, worldNormal, overlap, worldContactPoint);
		return true;
	}

	return false;
}

bool CollisionManager::checkOBBOBB(StructureBlock& a, StructureBlock& b)
{
	// 1. Get axes to check (Normal vectors of the faces(
	sf::Vector2f axes[4] = {
		rotateVector({1.f, 0.f}, a.getRotation() * (Config::PI / 180.f)),
		rotateVector({0.f, 1.f}, a.getRotation() * (Config::PI / 180.f)),
		rotateVector({1.f, 0.f}, b.getRotation() * (Config::PI / 180.f)),
		rotateVector({0.f, 1.f}, b.getRotation() * (Config::PI / 180.f))
	};

	float minOverlab = std::numeric_limits<float>::max();
	sf::Vector2f smallestAxis;

	// 2. Project verices onto each axis
	for (int i = 0; i < 4; i++) {
		sf::Vector2f axis = axes[i];

		float length = std::sqrt(axis.x * axis.x + axis.y * axis.y);
		if (length > 0.0) axis /= length;

		// Helper to project a shape onto an axis and ger [min, max]
		float minA, maxA, minB, maxB;
		projectRectangle(a, axis, minA, maxA);
		projectRectangle(b, axis, minB, maxB);

		// 3. Check for gap (The "Separating Axis")
		if (maxA < minB || maxB < minA) return false;

		// Calculate overlab on this axis
		float overlap = std::min(maxA, maxB) - std::max(minA, minB);
		if (overlap < minOverlab) {
			minOverlab = overlap;
			smallestAxis = axis;
		}
	}

	// 4. Resolution
	// Ensure the normal points from B to A
	sf::Vector2f d = a.getPosition() - b.getPosition();
	if (d.x * smallestAxis.x + d.y * smallestAxis.y < 0) smallestAxis = -smallestAxis;

	// For OBB-OBB, the contact point is complex. 
	sf::Vector2f contactPoint(0, 0);
	int contactCount = 0;

	auto findContacts = [&](StructureBlock& bodyA, StructureBlock& bodyB) {
		auto vertices = bodyA.getVertices();
		for (const auto& v : vertices) {
			if (isPointInsideOBB(v, bodyB)) {
				contactPoint += v;
				contactCount++;
			}
		}
	};

	findContacts(a, b);
	findContacts(b, a);

	if (contactCount > 0) {
		contactPoint /= static_cast<float>(contactCount);
	} else {
		contactPoint = a.getPosition() - smallestAxis * (minOverlab * 0.5f);
	}

	resolveCollision(a, b, smallestAxis, minOverlab, contactPoint);
	return true;
}

void CollisionManager::resolveCollision(Entity& a, Entity& b, sf::Vector2f normal, float overlap, sf::Vector2f contactPoint) {
	float invMassSum = a.getInvMass() + b.getInvMass();
	if (invMassSum == 0) return;	// Both are static

	// 1. Positional correction
	float penetration = std::max(overlap - Config::SLOP, 0.0f);
	sf::Vector2f correction = (penetration / invMassSum) * Config::PENETRATION_CORRETION_PERCENTAGE * normal;

	a.setPosition(a.getPosition() + correction * a.getInvMass());
	b.setPosition(b.getPosition() - correction * b.getInvMass());

	printf("normal X: %f\n", normal.x);
	if (std::abs(normal.x) < 0.3f && normal.y < 0.f) {
		sf::Vector2f normalForce(0.f, -a.getMass() * Config::GRAVITY);
		printf("normalForce X: %f\n", normalForce.x);
		printf("normalForce X: %f\n", normalForce.y);
		a.applyForce(normalForce, contactPoint);
	}

	// 2. Contact vectors
	sf::Vector2f ra = contactPoint - a.getPosition();
	sf::Vector2f rb = contactPoint - b.getPosition();

	auto getContactVelocity = [](Entity& e, sf::Vector2f r) {
		return e.getVelocity() + sf::Vector2f(-e.getAngularVelocity() * r.y, e.getAngularVelocity() * r.x);
	};

	sf::Vector2f relVel = getContactVelocity(a, ra) - getContactVelocity(b, rb);
	
	float velAlongNormal = relVel.x * normal.x + relVel.y * normal.y;

	if (velAlongNormal > 0.0f) return;	// Already moving apart

	// S3. Normal impulse (bounce)
	// Cross product for rotation (r x n)^2
	float raCrossN = ra.x * normal.y - ra.y * normal.x;
	float rbCrossN = rb.x * normal.y - rb.y * normal.x;

	// The full denomimnator including rotation
	float denom = invMassSum +
		(raCrossN * raCrossN) * a.getInvInertia() +
		(rbCrossN * rbCrossN) * b.getInvInertia();

	float e = std::sqrt(a.getRestitution() * b.getRestitution());

	if (velAlongNormal > -Config::RESTITUTION_VELOCITY_THRESHOLD)
		return;

	float j = -(1.0f + e) * velAlongNormal;
	j /= denom;

	if (j <= 0.0f) return;

	j = std::min(j, 1000.0f);

	sf::Vector2f impulse = j * normal;

	// Apply linear velocity
	a.setVelocity(a.getVelocity() + impulse * a.getInvMass());
	b.setVelocity(b.getVelocity() - impulse * b.getInvMass());

	// Apply angular velocity: w = w + (r x J) / I
	float torqueA = ra.x * impulse.y - ra.y * impulse.x;
	float torqueB = rb.x * impulse.y - rb.y * impulse.x;

	a.setAngularVelocity(a.getAngularVelocity() + torqueA * a.getInvInertia());
	b.setAngularVelocity(b.getAngularVelocity() - torqueB * b.getInvInertia());

	// recalculatethe relative velocity
	relVel = getContactVelocity(a, ra) - getContactVelocity(b, rb);
	velAlongNormal = relVel.x * normal.x + relVel.y * normal.y;

	// 4. Friction
	sf::Vector2f tangent = relVel - normal * velAlongNormal;

	float tLen = std::sqrt(tangent.x * tangent.x + tangent.y * tangent.y);
	if (tLen < Config::EPSILON) return;

	tangent /= tLen;

	// Reltive velocity impulse magnitude
	float vt = relVel.x * tangent.x + relVel.y * tangent.y;

	// Friction impuls
	// Cross product for tangent: (r x n)^2
	float raCrossT = ra.x * tangent.y - ra.y * tangent.x;
	float rbCrossT = rb.x * tangent.y - rb.y * tangent.x;

	float frictionDenom = invMassSum +
		(raCrossT * raCrossT) * a.getInvInertia() +
		(rbCrossT * rbCrossT) * b.getInvInertia();

	float jt = -vt / frictionDenom;

	float muS = std::sqrt(a.getStaticFriction() * b.getStaticFriction());
	float muD = std::sqrt(a.getDynamicFriction() * b.getDynamicFriction());

	sf::Vector2f frictionImpulse;

	if (std::abs(jt) < j * muS) {
		// Static friction
		frictionImpulse = jt * tangent;
	}
	else {
		// Dynamic friction
		frictionImpulse = -j * muD * tangent;
	}

	// Apply friction
	a.setVelocity(a.getVelocity() + frictionImpulse * a.getInvMass());
	b.setVelocity(b.getVelocity() - frictionImpulse * b.getInvMass());

	// Apply Angula: w =w + (r x J) / I
	float torqueFA = (ra.x * frictionImpulse.y - ra.y * frictionImpulse.x);
	float torqueFB = (rb.x * -frictionImpulse.y - rb.y * -frictionImpulse.x);

	a.setAngularVelocity(a.getAngularVelocity() + torqueFA * a.getInvInertia());
	b.setAngularVelocity(b.getAngularVelocity() + torqueFB * b.getInvInertia());

	// 5. Clamp static bodies
	if (a.getInvMass() == 0) {
		a.setAngularVelocity(0);
	}
	if (b.getInvMass() == 0) {
		b.setAngularVelocity(0);
	}
}

void CollisionManager::projectRectangle(const StructureBlock& rect, sf::Vector2f axis, float& min, float& max) {
	// Get the 4 corners of the rectangle
	sf::Vector2f p = rect.getPosition();
	sf::Vector2f h = rect.getSize() * 0.5f;
	float rad = rect.getRotation() * (Config::PI / 180.f);

	sf::Vector2f corners[4] = {
		p + rotateVector({-h.x, -h.y}, rad),
		p + rotateVector({ h.x, -h.y}, rad),
		p + rotateVector({ h.x,  h.y}, rad),
		p + rotateVector({-h.x,  h.y}, rad)
	};

	// Dot product each corner with the axis to find the range
	min = max = (corners[0].x * axis.x + corners[0].y * axis.y);
	for (int i = 0; i < 4; i++) {
		float projection = (corners[i].x * axis.x + corners[i].y * axis.y);
		if (projection < min) min = projection;
		if (projection > max) max = projection;
	}
}

bool CollisionManager::isPointInsideOBB(sf::Vector2f p, const StructureBlock& rect) {
	sf::Vector2f localP = inverseRotateVector(p - rect.getPosition(), rect.getRotation() * (Config::PI / 180.f));
	sf::Vector2f h = rect.getSize() * 0.5f;
	return (std::abs(localP.x) <= h.x && std::abs(localP.y) <= h.y);
}

//  FOR DEBUGING
bool CollisionManager::showDebugForces = false;

void CollisionManager::drawDebugForces(sf::RenderWindow& window, const std::vector<std::unique_ptr<Projectile>>& projectiles, const std::vector<std::unique_ptr<StructureBlock>>& blocks, StructureBlock* floor) {
	if (!showDebugForces) return;

	auto drawEntityForce = [&](Entity* e) {
		if (!e || e->getInvMass() == 0) return;

		sf::Vector2f forceVector = e->getLastTotalForce();

		drawArrow(window, e->getPosition(), forceVector * 0.1f, sf::Color::White);
	};

	for (auto& p : projectiles) drawEntityForce(p.get());
	for (auto& b : blocks) drawEntityForce(b.get());
}

void CollisionManager::drawArrow(sf::RenderWindow& window, sf::Vector2f start, sf::Vector2f force, sf::Color color) {
	float actualLength = std::sqrt(force.x * force.x + force.y * force.y);

	if (actualLength < Config::EPSILON) return;

	float visualLength = std::clamp(actualLength * 0.05f, 20.0f, 100.0f);

	sf::Vector2f direction = force / actualLength;
	sf::Vector2f visualEnd = start + (direction * visualLength);

	sf::Vertex line[] = {
		sf::Vertex(start, color),
		sf::Vertex(visualEnd, color)
	};
	window.draw(line, 2, sf::Lines);

	sf::CircleShape tip(4.0f);
	tip.setFillColor(color);
	tip.setOrigin(4.0f, 4.0f);
	tip.setPosition(visualEnd);

	/*float angle = std::atan2(direction.y, direction.x) * (180.0f / Config::PI);
	tip.setRotation(angle + 90.0f);*/

	window.draw(tip);
}
