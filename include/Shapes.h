#pragma once
#include "RigidBody.h"

class Box : public RigidBody {
public:
    sf::Vector2f size;
    Box(float x, float y, float w, float h, float m, Config::Material mat, std::string n);
    void draw(sf::RenderWindow& window) override;
};

class Ball : public RigidBody {
public:
    float radius;
    Ball(float x, float y, float r, float m, Config::Material mat, std::string n);
    void draw(sf::RenderWindow& window) override;
};

class Triangle : public RigidBody {
public:
    float side;
    Triangle(float x, float y, float s, float m, Config::Material mat, std::string n);
    void draw(sf::RenderWindow& window) override;
};