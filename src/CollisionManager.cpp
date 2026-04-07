#include "CollisionManager.h"

void CollisionManager::handleCollisions(std::vector<std::unique_ptr<Projectile>>& projectiles, std::vector<std::unique_ptr<StructureBlock>>& blocks) {
	for (int k = 0; k < Config::INERATIONS; k++) {
		// Circle vs Block
		for (auto& p : projectiles) {
			for (auto& b : blocks) {
				checkCircleOBB(*p, *b);
			}
		}

		// Block vs Block
		for (size_t i = 0; i < blocks.size(); ++i) {
			for (size_t j = i + 1; j < blocks.size(); ++j) {
				checkOBBOBB(*blocks[i], *blocks[j]);
			}
		}
	}
}

bool CollisionManager::checkCircleOBB(Projectile& circle, StructureBlock& block)
{
	// --- STAGE 1: Extract Physics Data ---
	sf::Vector2f center = circle.getPosition();
	sf::Vector2f rectPos = block.getPosition();
	sf::Vector2f size = block.getSize();

	// Convert degrees to radians: rad = deg * (PI / 180)
	float angleRad = block.getRotation() * (Config::PI / 180.0f);  // [rad]

	// --- STAGE 2: Local Space Transformation ---
	// Formula: Translate world point to relatice, then apply Inverse Rotation Matrix
	sf::Vector2f relativePos = center - rectPos;

	sf::Vector2f localVector = inverseRotateVector(relativePos, angleRad);

	// --- STAGE 3: Clamping (Finding Closest Point on AABB) ---
	// Find the point on the rectangle closest to the circle center in local space
	float halfW = size.x / 2.0f;
	float halfH = size.y / 2.0f;

	float closestX = std::max(-halfW, std::min(localVector.x, halfW));
	float closestY = std::max(-halfH, std::min(localVector.y, halfH));

	sf::Vector2f localContactPoint(closestX, closestY);

	// --- STAGE 4: Distance Check
	// Formula: Pythagorean Theorem in local space
	float dx = localVector.x - closestX;
	float dy = localVector.y - closestY;
	float distanceSquared = (dx * dx) + (dy * dy);
	float radius = circle.getRadius();

	if (distanceSquared < (radius * radius)) {
		float dist = std::sqrt(distanceSquared);

		// --- STAGE 5: Normal Reconstruction ---
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
	// --- STAGE 1: Get axes to check (Normal vectors of the faces) ---
	sf::Vector2f axes[4] = {
		rotateVector({1.f, 0.f}, a.getRotation() * (Config::PI / 180.f)),
		rotateVector({0.f, 1.f}, a.getRotation() * (Config::PI / 180.f)),
		rotateVector({1.f, 0.f}, b.getRotation() * (Config::PI / 180.f)),
		rotateVector({0.f, 1.f}, b.getRotation() * (Config::PI / 180.f))
	};

	float minOverlab = std::numeric_limits<float>::max();
	sf::Vector2f smallestAxis;

	// -- STAGE 2: Project verices onto each axis ---
	for (int i = 0; i < 4; i++) {
		sf::Vector2f axis = axes[i];

		float length = std::sqrt(axis.x * axis.x + axis.y * axis.y);
		if (length > 0.0) axis /= length;

		// Helper to project a shape onto an axis and ger [min, max]
		float minA, maxA, minB, maxB;
		projectRectangle(a, axis, minA, maxA);
		projectRectangle(b, axis, minB, maxB);

		// --- STAGE 3: Check for gap (The "Separating Axis") ---
		if (maxA < minB || maxB < minA) return false;

		// Calculate overlab on this axis
		float overlap = std::min(maxA, maxB) - std::max(minA, minB);
		if (overlap < minOverlab) {
			minOverlab = overlap;
			smallestAxis = axis;
		}
	}

	// --- STAGE 4: Resolution ---
	// Ensure the normal points from B to A
	sf::Vector2f d = a.getPosition() - b.getPosition();
	if (d.x * smallestAxis.x + d.y * smallestAxis.y < 0) smallestAxis = -smallestAxis;

	// For OBB-OBB, the contact point is complex. 
	// We can approximate it as the midpoint between the overlapping centers for now.
	sf::Vector2f contactPoint = a.getPosition() - smallestAxis * (minOverlab) * 0.5f;

	resolveCollision(a, b, smallestAxis, minOverlab, contactPoint);
	return true;
}

void CollisionManager::resolveCollision(Entity& a, Entity& b, sf::Vector2f normal, float overlap, sf::Vector2f contactPoint) {
	float penetration = std::max(overlap - Config::SLOP, 0.0f);
	if (penetration <= 0.0f) return;
	
	a.setSleeping(false);
	b.setSleeping(false);

	// --- STAGE 1: Positional Correction (Anti-Sinking) ---
	// Prevents objects from getting "stuck" inside each other due to floating point errors
	float invMassA = 1.0f / a.getMass();
	float invMassB = 1.0f / b.getMass();
	sf::Vector2f correction = (penetration / (invMassA + invMassB)) * Config::PENETRATION_CORRETION_PERCENTAGE * normal;

	a.setPosition(a.getPosition() + invMassA * correction);
	b.setPosition(b.getPosition() - invMassB * correction);

	// --- STAGE 2: Impulse Calculation ---
	// Formula: j = -(1 + e) * (v_rel . n) / (1/ma + 1/mb)
	sf::Vector2f relvel = a.getVelocity() - b.getVelocity();
	float velAlongNormal = relvel.x * normal.x + relvel.y * normal.y;

	if (velAlongNormal > -0.5f) return;  // Already moving apart

	float e = (std::abs(velAlongNormal) < Config::MIN_VELOCITY) ? 0.0f : Config::BOUNCE_RESTITUTION;
	float impulseMagnitude = -(1.0f + e) * velAlongNormal;  // Impulse Magnitude
	impulseMagnitude /= (invMassA + invMassB);

	//if (std::abs(velAlongNormal) < Config::MIN_VELOCITY) return;

	sf::Vector2f impulse = impulseMagnitude * normal;

	a.setVelocity(a.getVelocity() + impulse * invMassA);
	b.setVelocity(b.getVelocity() - impulse * invMassB);

	// --- STAGE 3: Angular Impulse (Torque) ---
	// Formula: Torque = r x F
	// Delta_Angular_Vel = (r x Impulse) / Inertia
	sf::Vector2f ra = contactPoint - a.getPosition();
	sf::Vector2f rb = contactPoint - b.getPosition();

	if (a.getInertia() > 0) {
		float torqueA = (ra.x * impulse.y) - (ra.y * impulse.x);
		a.setAngularVelocity(a.getAngularVelocity() + (torqueA / a.getInertia()));
	}

	if (b.getInertia() > 0) {
		float torqueB = (rb.x * -impulse.y) - (rb.y * -impulse.x);
		b.setAngularVelocity(b.getAngularVelocity() + (torqueB / b.getInertia()));
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
