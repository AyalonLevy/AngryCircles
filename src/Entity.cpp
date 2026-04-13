#include "Entity.h"

// Currently, logic is handled in the header or by children.
// We keep this file to satisfy the build system and for future shared logic.

void Entity::applyForce(sf::Vector2f force, sf::Vector2f contactPoint) {
	m_force += force;

	// Torque = r x F
	sf::Vector2f r = contactPoint - m_position;
	m_torque += (r.x * force.y - r.y * force.x);
}

void Entity::integrate(float dt) {
	if (m_invMass == 0)
		return;

	sf::Vector2f netForce = m_force + m_supportForce;
	printf("m_force: (%f, %f)\n", m_force.x, m_force.y);
	printf("m_supportForce: (%f, %f)\n", m_supportForce.x, m_supportForce.y);
	printf("netForce: (%f, %f)\n", netForce.x, netForce.y);

	if (std::abs(netForce.y) < Config::EPSILON) netForce.y = 0.0f;
	if (std::abs(netForce.x) < Config::EPSILON) netForce.x = 0.0f;

	// Update Velocities
	sf::Vector2f acceleration = m_force * m_invMass;
	sf::Vector2f nextvelocity = m_velocity + acceleration * dt;

	m_velocity += acceleration * dt;
	float speed = std::sqrt(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y);

	// Linear clamp
	if (speed > Config::MAX_LINEAR_VELOCITY)
		m_velocity = (m_velocity / speed) * Config::MAX_LINEAR_VELOCITY;

	// Angular clamp
	if (std::abs(m_angularVelocity) > Config::MAX_ANGULAR_VELOCITY)
		m_angularVelocity = (m_angularVelocity > 0 ? 1 : -1) * Config::MAX_ANGULAR_VELOCITY;

	float angularAccel = m_torque * m_invInertia;
	m_angularVelocity += angularAccel * dt;

	// Apply Damping
	m_velocity *= Config::FRICTION;
	m_angularVelocity *= Config::ANGULAR_DAMPING;

	// Update Positions
	m_position += m_velocity * dt;
	m_angle += m_angularVelocity * dt;

	// Sleep logic (stopping micro-vinrations)
	//printf("x velocity: %f\n", m_velocity.x);
	if (std::abs(m_velocity.x) < Config::MIN_LINEAR_VELOCITY) m_velocity.x = 0;
	//printf("y velocity: %f\n", m_velocity.y);
	if (std::abs(m_velocity.y) < Config::MIN_LINEAR_VELOCITY) m_velocity.y = 0;
	//printf("angular velocity: %f\n", m_angularVelocity);
	if (std::abs(m_angularVelocity) < Config::MIN_ANGULAR_VELOCITY) m_angularVelocity = 0;

	// Reset accumulators
	m_force = { 0.f, 0.f };
	m_torque = 0.f;
	resetSupport();
}