#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <iostream>

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;
const float GRAVITY = 0.6f;
const float JUMP_FORCE = -11.5f;
const float PIPE_SPEED = 4.0f;
const int PIPE_WIDTH = 80;
const int PIPE_GAP = 300;

class Bird {
public:
    sf::RectangleShape shape;
    float velocity;

    Bird() {
        shape.setSize(sf::Vector2f(40, 40));
        shape.setFillColor(sf::Color::Yellow);
        shape.setPosition(100, WINDOW_HEIGHT / 2);
        velocity = 0.0f;
    }

    void update() {
        velocity += GRAVITY;
        shape.move(0, velocity);
    }

    void jump() {
        velocity = JUMP_FORCE;
    }

    void reset() {
        shape.setPosition(100, WINDOW_HEIGHT / 2);
        velocity = 0.0f;
    }
};

class Pipe {
public:
    sf::RectangleShape topPipe;
    sf::RectangleShape bottomPipe;
    float x;

    Pipe(float startX) {
        x = startX;
        topPipe.setSize(sf::Vector2f(PIPE_WIDTH, rand() % (WINDOW_HEIGHT - PIPE_GAP)));
        bottomPipe.setSize(sf::Vector2f(PIPE_WIDTH, WINDOW_HEIGHT - PIPE_GAP - topPipe.getSize().y));

        topPipe.setFillColor(sf::Color::Green);
        bottomPipe.setFillColor(sf::Color::Green);

        topPipe.setPosition(x, 0);
        bottomPipe.setPosition(x, topPipe.getSize().y + PIPE_GAP);
    }

    void update() {
        x -= PIPE_SPEED;
        topPipe.setPosition(x, 0);
        bottomPipe.setPosition(x, topPipe.getSize().y + PIPE_GAP);
    }

    bool isOffScreen() {
        return x + PIPE_WIDTH < 0;
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Flappy Bird Clone");
    window.setFramerateLimit(60);

    Bird bird;
    std::vector<Pipe> pipes;
    pipes.push_back(Pipe(WINDOW_WIDTH));

    sf::Clock clock;
    float spawnTimer = 0;
    bool isGameOver = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space && !isGameOver) {
                bird.jump();
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R && isGameOver) {
                // Reset game
                bird.reset();
                pipes.clear();
                pipes.push_back(Pipe(WINDOW_WIDTH));
                isGameOver = false;
            }
        }

        if (!isGameOver) {
            bird.update();

            // Spawn pipes
            spawnTimer += clock.restart().asSeconds();
            if (spawnTimer > 1.5f) {
                pipes.push_back(Pipe(WINDOW_WIDTH));
                spawnTimer = 0;
            }

            // Update pipes
            for (auto& pipe : pipes) {
                pipe.update();
            }

            // Remove off-screen pipes
            if (!pipes.empty() && pipes.front().isOffScreen()) {
                pipes.erase(pipes.begin());
            }

            // Check collisions
            for (auto& pipe : pipes) {
                if (bird.shape.getGlobalBounds().intersects(pipe.topPipe.getGlobalBounds()) ||
                    bird.shape.getGlobalBounds().intersects(pipe.bottomPipe.getGlobalBounds()) ||
                    bird.shape.getPosition().y < 0 ||
                    bird.shape.getPosition().y > WINDOW_HEIGHT) {
                    isGameOver = true;
                }
            }
        }

        // Render
        window.clear(sf::Color::Cyan);

        window.draw(bird.shape);
        for (auto& pipe : pipes) {
            window.draw(pipe.topPipe);
            window.draw(pipe.bottomPipe);
        }

        window.display();
    }

    return 0;
}
