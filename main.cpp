#include <SFML/Graphics.hpp>

int main() {
    // Configuration constants
    const unsigned int WINDOW_WIDTH = 800;
    const unsigned int WINDOW_HEIGHT = 600;
    const float SQUARE_SIZE = 100.0f;

    // Initialize the window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "C++ Setup Test");

    // Create the square (Rectangle)
    sf::RectangleShape testSquare(sf::Vector2f(SQUARE_SIZE, SQUARE_SIZE));
    testSquare.setFillColor(sf::Color::Green);

    // Position it in the center
    testSquare.setPosition((WINDOW_WIDTH / 2.0f) - (SQUARE_SIZE / 2.0f),
        (WINDOW_HEIGHT / 2.0f) - (SQUARE_SIZE / 2.0f));

    // Basic Game Loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Rendering logic
        window.clear(sf::Color::Black);
        window.draw(testSquare);
        window.display();
    }

    return 0;
}