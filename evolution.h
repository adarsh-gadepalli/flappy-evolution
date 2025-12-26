#ifndef EVOLUTION_H
#define EVOLUTION_H

#include "neural_network.h"
#include "simulation.h"
#include <vector>
#include <random>

class Evolution {
private:
    std::vector<NeuralNetwork> population;
    std::vector<float> fitness;
    std::vector<int> topology;
    
    int populationSize;
    int gamesPerEvaluation;
    float mutationRate;
    float mutationStrength;
    float eliteRatio;
    int tournamentSize;
    
    std::mt19937& gen;
    std::uniform_real_distribution<float>& gapSize;
    std::uniform_real_distribution<float>& gapY;
    
    // Evaluate a single agent
    float evaluateAgent(NeuralNetwork& agent);
    
    // Tournament selection: pick random agents, return best
    int tournamentSelect();
    
public:
    // Constructor
    Evolution(int populationSize,
              const std::vector<int>& topology,
              int gamesPerEvaluation,
              float mutationRate,
              float mutationStrength,
              float eliteRatio,
              int tournamentSize,
              std::mt19937& gen,
              std::uniform_real_distribution<float>& gapSize,
              std::uniform_real_distribution<float>& gapY);
    
    // Run one generation: evaluate, select, crossover, mutate
    void evolve();
    
    // Get best agent
    NeuralNetwork getBestAgent() const;
    
    // Get best fitness
    float getBestFitness() const;
    
    // Get average fitness
    float getAverageFitness() const;
    
    // Get current generation statistics
    void getStatistics(float& best, float& average, float& worst) const;
};

#endif

