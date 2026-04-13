#include "RigidBody.h"

int RigidBody::nextID = 0; // Initialize the static counter

RigidBody::RigidBody(float m, Config::Material mat, std::string n)
    : mass(m), material(mat), name(n) {
    id = nextID++;
    invMass = (mass > 0) ? 1.0f / mass : 0.0f;
}

void RigidBody::applyForce(sf::Vector2f force, sf::Vector2f worldContactPoint) {
    if (invMass == 0) return;
    forceAccumulator += force;
    sf::Vector2f r = worldContactPoint - position;
    torqueAccumulator += (r.x * force.y - r.y * force.x);
}

void RigidBody::integrate(float dt) {
    if (invMass == 0) return;

    // 1. Linear: a = F/m
    sf::Vector2f acceleration = forceAccumulator * invMass;
    velocity += acceleration * dt;
    position += velocity * dt;

    // 2. Angular: alpha = T/I
    float angularAcceleration = torqueAccumulator * invInertia;
    angularVelocity += angularAcceleration * dt;
    rotation += angularVelocity * dt;

    // 3. Reset for next frame
    forceAccumulator = { 0.f, 0.f };
    torqueAccumulator = 0.0f;
}