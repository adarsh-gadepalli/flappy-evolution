#include "renderer.h"
#include "game_types.h"
#include <SFML/Graphics.hpp>

void render(sf::RenderWindow& window, const Bird& bird, 
            const std::vector<Pipe>& pipes, int score, 
            int highScore, GameState state, const sf::Font& font) {
    window.clear(sf::Color(135, 206, 235)); // Sky blue background
    
    // Draw ground
    sf::RectangleShape ground(sf::Vector2f(WINDOW_WIDTH, 50));
    ground.setPosition(sf::Vector2f(0, WINDOW_HEIGHT - 50));
    ground.setFillColor(sf::Color(34, 139, 34)); // Forest green
    window.draw(ground);
    
    if (state == GameState::START) {
        // Draw "Press Space to Start"
        sf::Text startText(font, "Press Space to Start", 50);
        sf::FloatRect textRect = startText.getLocalBounds();
        startText.setOrigin(sf::Vector2f(textRect.position.x + textRect.size.x/2.0f, textRect.position.y + textRect.size.y/2.0f));
        startText.setPosition(sf::Vector2f(WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f - 50));
        startText.setFillColor(sf::Color::White);
        startText.setOutlineColor(sf::Color::Black);
        startText.setOutlineThickness(3.0f);
        window.draw(startText);

        // Draw High Score
        sf::Text highScoreText(font, "High Score: " + std::to_string(highScore), 40);
        sf::FloatRect hsRect = highScoreText.getLocalBounds();
        highScoreText.setOrigin(sf::Vector2f(hsRect.position.x + hsRect.size.x/2.0f, hsRect.position.y + hsRect.size.y/2.0f));
        highScoreText.setPosition(sf::Vector2f(WINDOW_WIDTH/2.0f, WINDOW_HEIGHT/2.0f + 50));
        highScoreText.setFillColor(sf::Color::Yellow);
        highScoreText.setOutlineColor(sf::Color::Black);
        highScoreText.setOutlineThickness(2.0f);
        window.draw(highScoreText);
    } else {
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
        
        // Draw score
        sf::Text scoreText(font, std::to_string(score), 40);
        scoreText.setPosition(sf::Vector2f(20.0f, 20.0f));
        scoreText.setFillColor(sf::Color::White);
        scoreText.setOutlineColor(sf::Color::Black);
        scoreText.setOutlineThickness(2.0f);
        window.draw(scoreText);
    }
    
    window.display();
}