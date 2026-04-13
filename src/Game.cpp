//#include "Game.h"
//#include "CollisionManager.h"
//
//
//Game::Game()
//	: m_isAiming(false), m_shotsRemaining(3)
//{
//	initWindow();
//	initSlignshot();
//	spawnLevel();
//}
//
//void Game::initWindow() {
//	m_window.create(
//		sf::VideoMode(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT),
//		"Angry Circles - C++ Project",
//		sf::Style::Close | sf::Style::Titlebar
//	);
//
//	// Limits the framerate to the monitor's refreshrate to avoid 100% CPU usage
//	m_window.setFramerateLimit(60);
//
//	// Create a very wide, thin block for the floor
//	// Positioned at the center of the screen width, at FLOOR_Y
//	float floorWidth = (float)Config::SCREEN_WIDTH * 2.0f;
//	float floorHeight = 100.0f;
//
//	m_floor = std::make_unique<StructureBlock>(
//		(float)Config::SCREEN_WIDTH / 2.0f,
//		Config::FLOOR_Y + (floorHeight / 2.0f),
//		floorWidth, floorHeight,
//		0.0f,	// MASS = 0 makes it STATIC
//		0.9f,	// static friction
//		0.8f,	// dynamic friction
//		0.2f,	// almost no bounce
//		sf::Color(80, 50, 30)
//	);
//
//	m_floor->update(0.f);
//}
//
//void Game::initSlignshot() {
//	m_isAiming = false;
//
//	m_currentMousePos = Config::SLINGSHOT_POS;
//}
//
//void Game::processEvents() {
//	sf::Event event;
//	while (m_window.pollEvent(event)) {
//		if (event.type == sf::Event::Closed)
//			m_window.close();
//		
//		// Start Aiming
//		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
//			if (m_shotsRemaining > 0) {
//				m_mousePressPos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
//				m_isAiming = true;
//			}
//		}
//
//		// Track Movement (For the rubber band visual)
//		if (event.type == sf::Event::MouseMoved) {
//			m_currentMousePos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
//		}
//
//		// Finilize Launch - Calculate velocity
//		if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
//			if (m_isAiming) {
//				m_isAiming = false;
//
//				m_currentMousePos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
//
//				sf::Vector2f launchVel = calculateLaunchVelocity();
//
//				// Create a new projectile at the press position and launch it
//				fireProjectile(launchVel);
//				m_shotsRemaining--;
//			}
//		}
//
//		if (event.type == sf::Event::KeyPressed) {
//			if (event.key.code == sf::Keyboard::F) {
//				CollisionManager::showDebugForces = !CollisionManager::showDebugForces;
//			}
//		}
//	}
//}
//
//sf::Vector2f Game::calculateLaunchVelocity()
//{
//	// Vector from Press to Release
//	sf::Vector2f rawVector = m_mousePressPos - m_currentMousePos;
//
//	// Apply sensitivity
//	sf::Vector2f velocity = rawVector * Config::SLINGSHOT_SENSITIVITY;
//
//	// Magnitude calculation
//	float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
//
//	// Clamping logic
//	if (speed > Config::MAX_LAUNCH_POWER) {
//		velocity = (velocity / speed) * Config::MAX_LAUNCH_POWER;
//	}
//	
//	return velocity;
//}
//
//void Game::fireProjectile(sf::Vector2f velocity)
//{
//	auto newBall = std::make_unique<Projectile>(
//		Config::SLINGSHOT_POS.x,
//		Config::SLINGSHOT_POS.y,
//		Config::STANDARD_BALL.radius,
//		Config::STANDARD_BALL.mass,
//		Config::STANDARD_BALL.staticFriction,
//		Config::STANDARD_BALL.dynamicFriction,
//		Config::STANDARD_BALL.restitution,
//		Config::STANDARD_BALL.color
//	);
//
//	newBall->launch(velocity);
//	m_projectiles.push_back(std::move(newBall));
//}
//
//void Game::spawnLevel() {
//	for (int i = 0; i < Config::LEVEL_1.rows; ++i) {
//		for (int j = 0; j < Config::LEVEL_1.cols; ++j) {
//			float x = Config::LEVEL_1.startPos.x + (j * (Config::WOOD_BLOCK.width + Config::LEVEL_1.spacing));
//			float y = Config::LEVEL_1.startPos.y - (i * (Config::WOOD_BLOCK.height + Config::LEVEL_1.spacing));
//
//			m_blocks.push_back(std::make_unique<StructureBlock>(
//				x, y,
//				Config::WOOD_BLOCK.width,
//				Config::WOOD_BLOCK.height,
//				Config::WOOD_BLOCK.mass,
//				Config::WOOD_BLOCK.staticFriction,
//				Config::WOOD_BLOCK.dynamicFriction,
//				Config::WOOD_BLOCK.restitution,
//				Config::WOOD_BLOCK.color
//			));
//		}
//	}
//}
//
//void Game::render()
//{
//	m_window.clear(sf::Color(25, 25, 30));  // Dark blue-ish night sky - TODO: add background
//
//	// Draw the floor
//	if (m_floor) {
//		m_floor->draw(m_window);
//	}
//
//	// Draw all projectiles
//	for (auto& p : m_projectiles) {
//		p->draw(m_window);
//	}
//
//	// Draw all blocks
//	for(auto& b : m_blocks) {
//		b->draw(m_window);
//	}
//
//	// Draw the "Slingshot" (just a simple line for now)
//	if (m_isAiming) {
//		// Calculate the relative displacement of the mouse
//		sf::Vector2f mouseDisplacement = m_currentMousePos - m_mousePressPos;
//
//		// The "Visual" end of the rubber band is the Nozzle + that displacement
//		sf::Vector2f visualEndOfBand = Config::SLINGSHOT_POS + mouseDisplacement;
//
//		sf::Vertex line[] = {
//			sf::Vertex(Config::SLINGSHOT_POS, sf::Color(150, 75, 0)),
//			sf::Vertex(visualEndOfBand, sf::Color::White)
//		};
//		m_window.draw(line, 2, sf::Lines);
//	}
//
//	CollisionManager::drawDebugForces(m_window, m_projectiles, m_blocks, m_floor.get());
//
//	m_window.display();
//}
//
//void Game::update(float dt) {
//	std::vector<sf::Vector2f> startVelBlocks(m_blocks.size());
//	std::vector<sf::Vector2f> startVelProjectiles(m_projectiles.size());
//
//	for (size_t i = 0; i < m_blocks.size(); ++i) startVelBlocks[i] = m_blocks[i]->getVelocity();
//	for (size_t i = 0; i < m_projectiles.size(); ++i) startVelProjectiles[i] = m_projectiles[i]->getVelocity();
//
//	// Apply force
//	auto applyForces = [&](Entity* e) {
//		sf::Vector2f startVel = e->getVelocity();
//		if (e->getInvMass() > 0) {
//			e->applyForce({ 0.f, e->getMass() * Config::GRAVITY }, e->getPosition());
//		}
//	};
//
//	if (m_floor) applyForces(m_floor.get());
//	for (auto& p : m_projectiles) applyForces(p.get());
//	for (auto& b : m_blocks) applyForces(b.get());
//
//	// Solve collisions
//	CollisionManager::handleCollisions(m_projectiles, m_blocks, m_floor.get());
//
//	// Integrate
//	auto integratePass = [&](Entity* e) {
//		e->integrate(dt);
//		e->update(dt);
//	};
//	
//	if (m_floor) integratePass(m_floor.get());
//	for (auto& p : m_projectiles) integratePass(p.get());
//	for (auto& b : m_blocks) integratePass(b.get());
//
//	for (size_t i = 0; i < m_blocks.size(); ++i) {
//		sf::Vector2f deltaVel = m_blocks[i]->getVelocity() - startVelBlocks[i];
//		m_blocks[i]->setLastTotalForce((deltaVel / dt) * m_blocks[i]->getMass());
//	}
//	for (size_t i = 0; i < m_projectiles.size(); ++i) {
//		sf::Vector2f deltaVel = m_projectiles[i]->getVelocity() - startVelProjectiles[i];
//		m_projectiles[i]->setLastTotalForce((deltaVel / dt) * m_projectiles[i]->getMass());
//	}
//}
//
//
//void Game::run() {
//	sf::Clock clock;
//	while (m_window.isOpen()) {
//		// Calculate Delta Time
//		sf::Time elapsed = clock.restart();
//		float dt = elapsed.asSeconds();
//
//		processEvents();
//		update(dt);
//		render();
//	}
//}

#include "Game.h"
#include <iostream>

Game::Game() {
    m_window.create(sf::VideoMode(800, 600), "Newtonian Tower");
    m_floor = std::make_unique<Box>(400.f, 550.f, 800.f, 50.f, 0.f, Config::STEEL, "Floor");

    float startY = 500.f; // Just above floor
    for (int i = 0; i < 4; ++i) {
        m_bodies.push_back(std::make_unique<Box>(400.f, startY - (i * 50.f), 50.f, 50.f, 10.f, Config::WOOD, "Box_" + std::to_string(i)));
    }
    
    // Add the Triangle on top
    m_bodies.push_back(std::make_unique<Triangle>(400.f, startY - (4 * 50.f) - 150.0f, 50.f, 5.f, Config::WOOD, "Roof"));
}

void Game::update(float dt) {
    // We use the world's step to ensure the sequence is atomic
    m_world.step(m_bodies, m_floor.get(), dt);
}

void Game::render() {
    m_window.clear(sf::Color(30, 30, 30));

    if (m_floor) m_floor->draw(m_window);
    for (auto& b : m_bodies) b->draw(m_window);

    m_window.display();
}

void Game::run() {
    sf::Clock clock;
    while (m_window.isOpen()) {
        // 1. Handle all user input (Clicks, closing window, etc.)
        processEvents(); 

        // 2. Physics Step
        float dt = clock.restart().asSeconds();
        if (dt > 0.0166f) dt = 0.0166f;

        update(dt);
        
        // 3. Draw
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    // Standard SFML pattern: poll all events in the queue
    while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_window.close();
        }

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));

            // Create a ball at mouse position
            auto projectile = std::make_unique<Ball>(mousePos.x, mousePos.y, 15.f, 5.f, Config::STEEL, "Projectile");

            // Give it a high horizontal velocity toward the tower
            projectile->velocity = sf::Vector2f(800.f, -200.f);

            // Add it to the world
            m_bodies.push_back(std::move(projectile));
        }
    }
}