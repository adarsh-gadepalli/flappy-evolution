#include "simulation.h"
#include "game_types.h"
#include <algorithm>
#include <cmath>

// Simple rectangle intersection check (without SFML)
struct Rect {
    float x, y, width, height;
    
    bool intersects(const Rect& other) const {
        return x < other.x + other.width &&
               x + width > other.x &&
               y < other.y + other.height &&
               y + height > other.y;
    }
};

// Extract game state features for neural network input
std::vector<float> extractFeatures(const Bird& bird, const std::vector<Pipe>& pipes) {
    std::vector<float> features(5);
    
    // Normalize bird y position (0-1)
    features[0] = bird.y / (WINDOW_HEIGHT - 50.0f);
    
    // Normalize velocity (assuming range -10 to 10)
    features[1] = (bird.vy + 10.0f) / 20.0f;
    features[1] = std::max(0.0f, std::min(1.0f, features[1])); // Clamp to [0, 1]
    
    // Find next pipe
    float nextPipeX = WINDOW_WIDTH;
    float gapY = WINDOW_HEIGHT / 2.0f;
    float gapSize = 200.0f;
    
    for (const auto& pipe : pipes) {
        if (pipe.x > bird.x && pipe.x < nextPipeX) {
            nextPipeX = pipe.x;
            gapY = pipe.gapY;
            gapSize = pipe.gap;
        }
    }
    
    // Distance to next pipe (normalized)
    features[2] = (nextPipeX - bird.x) / WINDOW_WIDTH;
    
    // Gap center position (normalized)
    features[3] = gapY / (WINDOW_HEIGHT - 50.0f);
    
    // Vertical distance from bird to gap center (normalized)
    features[4] = (bird.y - gapY) / (WINDOW_HEIGHT - 50.0f);
    
    return features;
}

// Check if bird collides with pipes or boundaries
bool checkCollision(const Bird& bird, const std::vector<Pipe>& pipes) {
    // Check boundaries
    if (bird.y < 0 || bird.y + BIRD_SIZE * 2 > WINDOW_HEIGHT - 50) {
        return true;
    }
    
    // Check pipe collisions
    Rect birdRect = {bird.x, bird.y, BIRD_SIZE * 2, BIRD_SIZE * 2};
    
    for (const auto& pipe : pipes) {
        float gapTop = pipe.gapY - pipe.gap / 2.0f;
        float gapBottom = pipe.gapY + pipe.gap / 2.0f;
        
        // Top pipe
        if (gapTop > 0) {
            Rect topPipeRect = {pipe.x, 0.0f, PIPE_WIDTH, gapTop};
            if (birdRect.intersects(topPipeRect)) {
                return true;
            }
        }
        
        // Bottom pipe
        if (gapBottom < WINDOW_HEIGHT - 50) {
            float bottomPipeHeight = (WINDOW_HEIGHT - 50) - gapBottom;
            Rect bottomPipeRect = {pipe.x, gapBottom, PIPE_WIDTH, bottomPipeHeight};
            if (birdRect.intersects(bottomPipeRect)) {
                return true;
            }
        }
    }
    
    return false;
}

// Headless game simulation
GameResult simulateGame(
    std::mt19937& gen,
    std::uniform_real_distribution<float>& gapSize,
    std::uniform_real_distribution<float>& gapY,
    std::function<bool(const std::vector<float>&)> shouldFlap,
    int maxFrames) {
    
    // Initialize game state
    Bird bird;
    bird.x = 100.0f;
    bird.y = WINDOW_HEIGHT / 2.0f;
    bird.vx = 0.0f;
    bird.vy = 0.0f;
    
    std::vector<Pipe> pipes;
    int score = 0;
    int frames = 0;
    int pipeSpawnCounter = 0;
    const int PIPE_SPAWN_INTERVAL = 120;
    
    GameResult result;
    result.crashed = false;
    result.framesAlive = 0;
    result.score = 0;
    result.distanceTraveled = 0.0f;
    
    // Game loop
    while (frames < maxFrames && !result.crashed) {
        // Extract features and get decision from agent
        auto features = extractFeatures(bird, pipes);
        bool flap = shouldFlap(features);
        
        if (flap) {
            bird.vy = JUMP_VELOCITY;
        }
        
        // Update bird physics
        bird.vy += GRAVITY;
        bird.y += bird.vy;
        bird.x += bird.vx;
        
        // Check collision
        if (checkCollision(bird, pipes)) {
            result.crashed = true;
            result.framesAlive = frames;
            result.score = score;
            result.distanceTraveled = bird.x;
            break;
        }
        
        // Generate new pipes
        pipeSpawnCounter++;
        if (pipeSpawnCounter >= PIPE_SPAWN_INTERVAL) {
            Pipe pipe;
            pipe.x = WINDOW_WIDTH;
            pipe.gap = gapSize(gen);
            pipe.gapY = gapY(gen);
            pipe.passed = false;
            pipes.push_back(pipe);
            pipeSpawnCounter = 0;
        }
        
        // Update pipe positions
        for (auto& pipe : pipes) {
            pipe.x -= SCROLL_SPEED;
            
            // Check if passed
            if (!pipe.passed && pipe.x + PIPE_WIDTH < bird.x) {
                score++;
                pipe.passed = true;
            }
        }
        
        // Remove pipes that are off screen
        pipes.erase(
            std::remove_if(pipes.begin(), pipes.end(),
                [](const Pipe& p) { return p.x < -PIPE_WIDTH; }),
            pipes.end()
        );
        
        frames++;
    }
    
    // Game completed without crashing
    if (!result.crashed) {
        result.framesAlive = frames;
        result.score = score;
        result.distanceTraveled = bird.x;
    }
    
    return result;
}

