#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <SFML/Graphics.hpp>

struct Bird {
    float x, y;
    float vx, vy;
};

struct Pipe {
    float x;
    float gap;
    float gapY; // vertical position of gap center
};

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float PIPE_WIDTH = 60.0f;
const float BIRD_SIZE = 20.0f;
const float GRAVITY = 0.5f;
const float JUMP_VELOCITY = -8.0f;

void render(sf::RenderWindow& window, const Bird& bird, const std::vector<Pipe>& pipes, int score) {
    window.clear(sf::Color(135, 206, 235)); // Sky blue background
    
    // Draw ground
    sf::RectangleShape ground(sf::Vector2f(WINDOW_WIDTH, 50));
    ground.setPosition(sf::Vector2f(0, WINDOW_HEIGHT - 50));
    ground.setFillColor(sf::Color(34, 139, 34)); // Forest green
    window.draw(ground);
    
    // Draw pipes
    for (const auto& pipe : pipes) {
        if (pipe.x > -PIPE_WIDTH && pipe.x < WINDOW_WIDTH + PIPE_WIDTH) {
            float gapTop = pipe.gapY - pipe.gap / 2;
            float gapBottom = pipe.gapY + pipe.gap / 2;
            
            // Top pipe
            if (gapTop > 0) {
                sf::RectangleShape topPipe(sf::Vector2f(PIPE_WIDTH, gapTop));
                topPipe.setPosition(sf::Vector2f(pipe.x, 0));
                topPipe.setFillColor(sf::Color(0, 150, 0)); // Dark green
                window.draw(topPipe);
            }
            
            // Bottom pipe
            if (gapBottom < WINDOW_HEIGHT - 50) {
                float bottomPipeHeight = (WINDOW_HEIGHT - 50) - gapBottom;
                sf::RectangleShape bottomPipe(sf::Vector2f(PIPE_WIDTH, bottomPipeHeight));
                bottomPipe.setPosition(sf::Vector2f(pipe.x, gapBottom));
                bottomPipe.setFillColor(sf::Color(0, 150, 0)); // Dark green
                window.draw(bottomPipe);
            }
        }
    }
    
    // Draw bird
    sf::CircleShape birdShape(BIRD_SIZE);
    birdShape.setPosition(sf::Vector2f(bird.x, bird.y));
    birdShape.setFillColor(sf::Color(255, 200, 0)); // Yellow
    window.draw(birdShape);
    
    // Draw score (simple text rendering would require font loading, so using a simple indicator)
    // For now, we'll just display it in the console
    
    window.display();
}

int main() {
    // Create SFML window
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT)), "Flappy Bird");
    window.setFramerateLimit(60);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> gapSize(150.0f, 250.0f);
    std::uniform_real_distribution<float> gapY(200.0f, WINDOW_HEIGHT - 250.0f);

    Bird bird;
    bird.x = 100.0f;
    bird.y = WINDOW_HEIGHT / 2.0f;
    bird.vx = 2.0f;
    bird.vy = 0.0f;

    std::vector<Pipe> pipes;
    int score = 0;
    int pipeSpawnCounter = 0;
    const int PIPE_SPAWN_INTERVAL = 120; // frames

    sf::Clock clock;
    
    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Space) {
                    bird.vy = JUMP_VELOCITY; // Jump/flap
                }
            }
        }

        // Update bird physics
        bird.vy += GRAVITY;
        bird.y += bird.vy;
        bird.x += bird.vx;

        // Check bird bounds
        if (bird.y < 0) {
            bird.y = 0;
            bird.vy = 0;
        }
        if (bird.y + BIRD_SIZE * 2 > WINDOW_HEIGHT - 50) {
            bird.y = WINDOW_HEIGHT - 50 - BIRD_SIZE * 2;
            bird.vy = 0;
            // Game over - could add game over logic here
        }

        // Generate new pipes
        pipeSpawnCounter++;
        if (pipeSpawnCounter >= PIPE_SPAWN_INTERVAL) {
            Pipe pipe;
            pipe.x = WINDOW_WIDTH;
            pipe.gap = gapSize(gen);
            pipe.gapY = gapY(gen);
            pipes.push_back(pipe);
            pipeSpawnCounter = 0;
        }

        // Update pipe positions (move left)
        for (auto& pipe : pipes) {
            pipe.x -= bird.vx;
        }

        // Remove pipes that are off screen
        for (auto it = pipes.begin(); it != pipes.end(); ) {
            if (it->x < -PIPE_WIDTH) {
                it = pipes.erase(it);
                score++; // Increment score when pipe passes
            } else {
                it++;
            }
        }
        
        // Render everything
        render(window, bird, pipes, score);
    }

    std::cout << "Final score: " << score << "\n";
    return 0;
}