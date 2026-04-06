#include "Game.h"


Game::Game()
	: m_isAiming(false), m_shotsRemaining(3)
{
	initWindow();
	initSlignshot();
	spawnLevel();
}

void Game::initWindow() {
	m_window.create(
		sf::VideoMode(Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT),
		"Angry Circles - C++ Project",
		sf::Style::Close | sf::Style::Titlebar
	);

	// Limits the framerate to the monitor's refreshrate to avoid 100% CPU usage
	m_window.setFramerateLimit(60);
}

void Game::initSlignshot() {
	m_isAiming = false;

	m_currentMousePos = Config::SLINGSHOT_POS;
}

void Game::processEvents() {
	sf::Event event;
	while (m_window.pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			m_window.close();
		
		// Start Aiming
		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
			if (m_shotsRemaining > 0) {
				m_mousePressPos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
				m_isAiming = true;
			}
		}

		// Track Movement (For the rubber band visual)
		if (event.type == sf::Event::MouseMoved) {
			m_currentMousePos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));
		}

		// Finilize Launch - Calculate velocity
		if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
			if (m_isAiming) {
				m_isAiming = false;

				m_currentMousePos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window));

				sf::Vector2f launchVel = calculateLaunchVelocity();

				// Create a new projectile at the press position and launch it
				fireProjectile(launchVel);
				m_shotsRemaining--;
			}
		}
	}
}

sf::Vector2f Game::calculateLaunchVelocity()
{
	// Vector from Press to Release
	sf::Vector2f rawVector = m_mousePressPos - m_currentMousePos;

	// Apply sensitivity
	sf::Vector2f velocity = rawVector * Config::SLINGSHOT_SENSITIVITY;

	// Magnitude calculation
	float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);

	// Clamping logic
	if (speed > Config::MAX_LAUNCH_POWER) {
		velocity = (velocity / speed) * Config::MAX_LAUNCH_POWER;
	}
	
	return velocity;
}

void Game::fireProjectile(sf::Vector2f velocity)
{
	auto newBall = std::make_unique<Projectile>(
		Config::SLINGSHOT_POS.x,
		Config::SLINGSHOT_POS.y,
		Config::STANDARD_BALL.radius,
		Config::STANDARD_BALL.mass,
		Config::STANDARD_BALL.color
	);

	newBall->launch(velocity);
	m_projectiles.push_back(std::move(newBall));
}

void Game::spawnLevel() {
	for (int i = 0; i < Config::LEVEL_1.rows; ++i) {
		for (int j = 0; j < Config::LEVEL_1.cols; ++j) {
			float x = Config::LEVEL_1.startPos.x + (j * (Config::WOOD_BLOCK.width + 5.0f));
			float y = Config::LEVEL_1.startPos.y - (i * (Config::WOOD_BLOCK.height + 5.0f));

			m_blocks.push_back(std::make_unique<StructureBlock>(
				x, y,
				Config::WOOD_BLOCK.width,
				Config::WOOD_BLOCK.height,
				Config::WOOD_BLOCK.mass,
				Config::WOOD_BLOCK.color
			));
		}
	}
}

void Game::render()
{
	m_window.clear(sf::Color(25, 25, 30));  // Dark blue-ish night sky - TODO: add background

	// Draw all projectiles
	for (auto& p : m_projectiles) {
		p->draw(m_window);
	}

	// Draw all blocks
	for(auto& b : m_blocks) {
		b->draw(m_window);
	}

	// Draw the "Slingshot" (just a simple line for now)
	if (m_isAiming) {
		sf::Vertex line[] = {
			sf::Vertex(Config::SLINGSHOT_POS, sf::Color(150, 75, 0)),
			sf::Vertex(m_currentMousePos, sf::Color::White)
		};
		m_window.draw(line, 2, sf::Lines);
	}

	m_window.display();
}

void Game::update(float dt) {
	for (auto& p : m_projectiles) {
		p->update(dt);
	}

	for (auto& b : m_blocks) {
		b->update(dt);
	}
}


void Game::run() {
	sf::Clock clock;
	while (m_window.isOpen()) {
		// Calculate Delta Time
		sf::Time elapsed = clock.restart();
		float dt = elapsed.asSeconds();

		processEvents();
		update(dt);
		render();
	}
}