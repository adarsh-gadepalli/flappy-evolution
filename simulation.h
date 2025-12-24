#ifndef SIMULATION_H
#define SIMULATION_H

#include "game_types.h"
#include <vector>
#include <random>

struct GameResult {
    int score;
    float distanceTraveled;
    int framesAlive;
    bool crashed;
};

// Forward declaration for NeuralNetwork (if needed)
class NeuralNetwork;

GameResult simulateGame(/* parameters */);
std::vector<float> extractFeatures(const Bird& bird, const std::vector<Pipe>& pipes);

#endif