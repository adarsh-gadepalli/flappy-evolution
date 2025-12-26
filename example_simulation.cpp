// Example: How to use the headless simulation
// This file demonstrates how to run a game simulation without rendering

#include "simulation.h"
#include <iostream>
#include <random>
#include <functional>

int main() {
    // Initialize random number generators
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> gapSize(150.0f, 250.0f);
    std::uniform_real_distribution<float> gapY(200.0f, WINDOW_HEIGHT - 250.0f);
    
    // Define a simple agent that flaps when bird is too low
    auto simpleAgent = [](const std::vector<float>& features) -> bool {
        // features[0] = bird y position (normalized)
        // features[1] = bird velocity (normalized)
        // features[2] = distance to next pipe (normalized)
        // features[3] = gap center position (normalized)
        // features[4] = vertical distance from bird to gap center (normalized)
        
        // Simple strategy: flap if bird is below 0.5 (middle of screen)
        return features[0] > 0.5f;
    };
    
    // Run simulation
    GameResult result = simulateGame(gen, gapSize, gapY, simpleAgent, 5000);
    
    // Print results
    std::cout << "Simulation Results:\n";
    std::cout << "  Score: " << result.score << "\n";
    std::cout << "  Frames Alive: " << result.framesAlive << "\n";
    std::cout << "  Distance Traveled: " << result.distanceTraveled << "\n";
    std::cout << "  Crashed: " << (result.crashed ? "Yes" : "No") << "\n";
    std::cout << "  Fitness: " << result.fitness() << "\n";
    
    return 0;
}

