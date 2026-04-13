#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include "RigidBody.h"
#include "Shapes.h"

class PhysicsWorld {
public:
    void step(std::vector<std::unique_ptr<RigidBody>>& bodies, RigidBody* floor, float dt);
private:
    void resolveCollisions(std::vector<std::unique_ptr<RigidBody>>& bodies, RigidBody* floor);
    void handlePairCollision(RigidBody& a, RigidBody& b);
};