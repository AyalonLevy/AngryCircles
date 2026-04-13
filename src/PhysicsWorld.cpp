#include "PhysicsWorld.h"

void PhysicsWorld::step(std::vector<std::unique_ptr<RigidBody>>& bodies, RigidBody* floor, float dt) {
    // 1. Gravity
    for (auto& b : bodies) {
        b->applyForce(sf::Vector2f(0, b->mass * Config::GRAVITY), b->position);
    }

    // 2. Normal Collision Pass (Handles Box-vs-Box and Ball-vs-Box)
    resolveCollisions(bodies, floor);

    // 3. FINAL FLOOR LOCK (The Safety Net)
    // This ensures no object ever ends the frame below the floor line.
    if (floor) {
        Box* f = static_cast<Box*>(floor);
        float floorTop = f->position.y - (f->size.y / 2.0f);

        for (auto& b : bodies) {
            float halfH = (b->name == "Projectile") ? 15.0f : 25.0f;
            float bottomEdge = b->position.y + halfH;

            // Inside the Final Floor Lock loop in PhysicsWorld::step
            if (bottomEdge > floorTop) {
                b->position.y = floorTop - halfH;

                // Calculate Normal Impulse magnitude (enough to stop velocity)
                float j_normal = b->mass * std::abs(b->velocity.y);
                if (b->velocity.y > 0) b->velocity.y = 0;

                // Apply Friction to horizontal velocity
                float mu = std::sqrt(b->material.staticFriction * floor->material.staticFriction);
                float frictionBudget = (j_normal + (b->mass * Config::GRAVITY * dt)) * mu;

                // Dampen x-velocity based on the "clamping" force of gravity
                float vx = b->velocity.x;
                float frictionImpulse = -vx * b->mass; // impulse to stop completely

                // Clamp the impulse
                frictionImpulse = std::max(-frictionBudget, std::min(frictionBudget, frictionImpulse));
                b->velocity.x += frictionImpulse / b->mass;
            }
        }
    }

    // 4. Integrate
    for (auto& b : bodies) b->integrate(dt);
}

void PhysicsWorld::resolveCollisions(std::vector<std::unique_ptr<RigidBody>>& bodies, RigidBody* floor) {
    // 1. Sort bodies by Y (Top to Bottom) for force propagation
    std::vector<RigidBody*> sortedBodies;
    for (auto& b : bodies) sortedBodies.push_back(b.get());

    std::sort(sortedBodies.begin(), sortedBodies.end(), [](RigidBody* a, RigidBody* b) {
        return a->position.y < b->position.y;
    });

    // 2. Resolve Body-vs-Body
    for (size_t i = 0; i < sortedBodies.size(); ++i) {
        for (size_t j = i + 1; j < sortedBodies.size(); ++j) {
            handlePairCollision(*sortedBodies[i], *sortedBodies[j]);
        }
    }

    // 3. Resolve Body-vs-Floor (THE FIX)
    // Ensure the floor check happens and reinforces the Projection
    if (floor) {
        for (auto* b : sortedBodies) {
            handlePairCollision(*b, *floor);
        }
    }
}

void PhysicsWorld::handlePairCollision(RigidBody& a, RigidBody& b) {
    sf::Vector2f diff = b.position - a.position;
    float distSq = diff.x * diff.x + diff.y * diff.y;

    float rA = (a.name == "Projectile") ? 15.0f : 25.0f;
    float rB = (b.invMass == 0) ? 25.0f : 25.0f;
    float minDistance = rA + rB;

    if (distSq < minDistance * minDistance) {
        float distance = std::sqrt(distSq);
        sf::Vector2f normal = (distance > 0) ? diff / distance : sf::Vector2f(0, -1);
        float overlap = minDistance - distance;

        float totalInvMass = a.invMass + b.invMass;
        if (totalInvMass <= 0) return;

        // 1. POSITION PROJECTION
        sf::Vector2f correction = normal * (overlap / totalInvMass);
        a.position -= correction * a.invMass;
        b.position += correction * b.invMass;

        // 2. DYNAMIC IMPULSE (Normal Direction)
        sf::Vector2f relVel = b.velocity - a.velocity;
        float velNormal = relVel.x * normal.x + relVel.y * normal.y;

        float j = 0;
        if (velNormal < 0) {
            float e = 0.2f;
            j = -(1.0f + e) * velNormal;
            j /= totalInvMass;

            sf::Vector2f impulse = j * normal;
            a.velocity -= a.invMass * impulse;
            b.velocity += b.invMass * impulse;
        }

        // 3. FRICTION (Tangential Direction)
        // Re-calculate relative velocity after normal impulse
        sf::Vector2f currentRelVel = b.velocity - a.velocity;

        // The tangent is perpendicular to the normal
        sf::Vector2f tangent = { -normal.y, normal.x };
        float velTan = currentRelVel.x * tangent.x + currentRelVel.y * tangent.y;

        if (std::abs(velTan) > 0.01f) {
            // Geometric mean of friction coefficients
            float mu = std::sqrt(a.material.staticFriction * b.material.staticFriction);

            // Impulse required to stop sliding
            float jt = -velTan / totalInvMass;

            // Friction Limit: Coulomb's Law |jt| <= j * mu
            // Note: We use the normal impulse 'j' to determine the friction budget
            float maxFriction = std::abs(j) * mu;
            jt = std::max(-maxFriction, std::min(maxFriction, jt));

            sf::Vector2f frictionImpulse = jt * tangent;
            a.velocity -= a.invMass * frictionImpulse;
            b.velocity += b.invMass * frictionImpulse;
        }
    }
}