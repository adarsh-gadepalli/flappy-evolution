#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <SFML/Graphics.hpp>

#include "game_types.h"
#include "renderer.h"

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
    bird.vx = 0.0f;
    bird.vy = 0.0f;

    std::vector<Pipe> pipes;
    int score = 0;
    int highScore = 0;
    GameState gameState = GameState::START;
    int pipeSpawnCounter = 0;
    const int PIPE_SPAWN_INTERVAL = 120; // frames

    sf::Font font;
    if (!font.openFromFile("/System/Library/Fonts/Helvetica.ttc")) {
        // Fallback or error handling
        if (!font.openFromFile("/Library/Fonts/Arial.ttf")) {
            std::cerr << "Error loading font\n";
        }
    }

    sf::Clock clock;
    
    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Space) {
                    if (gameState == GameState::START) {
                        // Reset game state
                        bird.y = WINDOW_HEIGHT / 2.0f;
                        bird.vy = 0.0f;
                        pipes.clear();
                        score = 0;
                        pipeSpawnCounter = 0;
                        gameState = GameState::PLAYING;
                        bird.vy = JUMP_VELOCITY;
                    } else {
                        bird.vy = JUMP_VELOCITY; // Jump/flap
                    }
                }
            }
        }

        if (gameState == GameState::PLAYING) {
            // Update bird physics
            bird.vy += GRAVITY;
            bird.y += bird.vy;
            bird.x += bird.vx;

            // Check collisions (Game Over)
            bool collision = false;

            // if bird hits the top or bottom of the screen, out of bounds
            if (bird.y < 0 || bird.y + BIRD_SIZE * 2 > WINDOW_HEIGHT - 50) {
                collision = true;
            }

            // Pipe collisions
            if (!collision) {
                sf::FloatRect birdRect(sf::Vector2f(bird.x, bird.y), sf::Vector2f(BIRD_SIZE * 2, BIRD_SIZE * 2));
                for (const auto& pipe : pipes) {
                    float gapTop = pipe.gapY - pipe.gap / 2.0f;
                    float gapBottom = pipe.gapY + pipe.gap / 2.0f;

                    if (gapTop > 0) {
                        sf::FloatRect pipeRect(sf::Vector2f(pipe.x, 0.0f), sf::Vector2f(PIPE_WIDTH, gapTop));

                        // if bird overlaps with top pipe, collision
                        if (birdRect.findIntersection(pipeRect)) {
                            collision = true;
                            break;
                        }
                    }
                    if (gapBottom < WINDOW_HEIGHT - 50) {
                        sf::FloatRect pipeRect(sf::Vector2f(pipe.x, gapBottom), sf::Vector2f(PIPE_WIDTH, (float)(WINDOW_HEIGHT - 50) - gapBottom));

                        // if bird overlaps with bottom pipe, collision
                        if (birdRect.findIntersection(pipeRect)) {
                            collision = true;
                            break;
                        }
                    }
                }
            }

            if (collision) {
                if (score > highScore) {
                    highScore = score;
                }
                gameState = GameState::START;
            } else {
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
                    pipe.x -= SCROLL_SPEED;
                    
                    // Check if passed
                    if (!pipe.passed && pipe.x + PIPE_WIDTH < bird.x) {
                        score++;
                        pipe.passed = true;
                    }
                }

                // Remove pipes that are off screen
                for (auto it = pipes.begin(); it != pipes.end(); ) {
                    if (it->x < -PIPE_WIDTH) {
                        it = pipes.erase(it);
                    } else {
                        it++;
                    }
                }
            }
        }
        
        // Render everything
        render(window, bird, pipes, score, highScore, gameState, font);
    }

    std::cout << "Final score: " << score << "\n";
    return 0;
}