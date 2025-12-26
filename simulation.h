#ifndef SIMULATION_H
#define SIMULATION_H

#include "game_types.h"
#include <vector>
#include <random>
#include <functional>

struct GameResult {
    int score;
    float distanceTraveled;
    int framesAlive;
    bool crashed;
    
    // Fitness function for evolutionary algorithm
    float fitness() const {
        return score * 100.0f + framesAlive + distanceTraveled * 0.1f;
    }
};

// Forward declaration for NeuralNetwork (if needed)
class NeuralNetwork;

// Headless game simulation
// shouldFlap: function that takes features and returns true if bird should flap
// maxFrames: maximum number of frames to simulate (prevents infinite loops)
GameResult simulateGame(
    std::mt19937& gen,
    std::uniform_real_distribution<float>& gapSize,
    std::uniform_real_distribution<float>& gapY,
    std::function<bool(const std::vector<float>&)> shouldFlap,
    int maxFrames = 10000);

// Extract game state features for neural network input
std::vector<float> extractFeatures(const Bird& bird, const std::vector<Pipe>& pipes);

#endif