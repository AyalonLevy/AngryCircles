#include "Shapes.h"

Box::Box(float x, float y, float w, float h, float m, Config::Material mat, std::string n)
    : RigidBody(m, mat, n), size(w, h) {
    position = { x, y };
    inertia = (1.0f / 12.0f) * mass * (size.x * size.x + size.y * size.y);
    invInertia = (inertia > 0) ? 1.0f / inertia : 0.0f;
}

void Box::draw(sf::RenderWindow& window) {
    sf::RectangleShape shape(size);
    shape.setOrigin(size.x / 2.0f, size.y / 2.0f);
    shape.setPosition(position);
    shape.setRotation(rotation * (180.f / Config::PI));

    // Draw static objects brown, dynamic objects green
    shape.setFillColor(invMass == 0 ? sf::Color(139, 69, 19) : sf::Color::Green);
    window.draw(shape);
}

Ball::Ball(float x, float y, float r, float m, Config::Material mat, std::string n)
    : RigidBody(m, mat, n), radius(r) {
    position = { x, y };
    // I = 1/2 * m * r^2
    inertia = 0.5f * mass * (radius * radius);
    invInertia = (inertia > 0) ? 1.0f / inertia : 0.0f;
}

void Ball::draw(sf::RenderWindow& window) {
    sf::CircleShape shape(radius);
    shape.setOrigin(radius, radius);
    shape.setPosition(position);
    shape.setRotation(rotation * (180.f / Config::PI));
    shape.setFillColor(sf::Color::Cyan);
    window.draw(shape);
}

Triangle::Triangle(float x, float y, float s, float m, Config::Material mat, std::string n)
    : RigidBody(m, mat, n), side(s) {
    position = { x, y };
    // I = (1/12) * m * s^2 for an equilateral triangle
    inertia = (1.0f / 12.0f) * mass * (side * side);
    invInertia = (inertia > 0) ? 1.0f / inertia : 0.0f;
}

void Triangle::draw(sf::RenderWindow& window) {
    sf::ConvexShape shape(3);
    // Simple equilateral triangle points
    float h = (std::sqrt(3.0f) / 2.0f) * side;
    shape.setPoint(0, sf::Vector2f(0, -h / 2.0f));
    shape.setPoint(1, sf::Vector2f(-side / 2.0f, h / 2.0f));
    shape.setPoint(2, sf::Vector2f(side / 2.0f, h / 2.0f));

    shape.setOrigin(0, 0);
    shape.setPosition(position);
    shape.setRotation(rotation * (180.f / Config::PI));
    shape.setFillColor(sf::Color::Yellow);
    window.draw(shape);
}