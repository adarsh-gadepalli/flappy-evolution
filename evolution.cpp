#include "evolution.h"
#include "simulation.h"
#include <algorithm>
#include <numeric>
#include <iostream>

// Constructor
Evolution::Evolution(int populationSize,
                     const std::vector<int>& topology,
                     int gamesPerEvaluation,
                     float mutationRate,
                     float mutationStrength,
                     float eliteRatio,
                     int tournamentSize,
                     std::mt19937& gen,
                     std::uniform_real_distribution<float>& gapSize,
                     std::uniform_real_distribution<float>& gapY)
    : populationSize(populationSize),
      gamesPerEvaluation(gamesPerEvaluation),
      mutationRate(mutationRate),
      mutationStrength(mutationStrength),
      eliteRatio(eliteRatio),
      tournamentSize(tournamentSize),
      gen(gen),
      gapSize(gapSize),
      gapY(gapY),
      topology(topology),
      population(populationSize, NeuralNetwork(topology, gen)),
      fitness(populationSize, 0.0f) {
}

// Evaluate a single agent by running multiple games
float Evolution::evaluateAgent(NeuralNetwork& agent) {
    // Create lambda function that uses the neural network
    auto agentFunction = [&agent](const std::vector<float>& features) -> bool {
        float output = agent.forward(features);
        return output > 0.5f; // Flap if output > 0.5
    };
    
    // Run multiple games and average fitness
    float totalFitness = 0.0f;
    for (int i = 0; i < gamesPerEvaluation; i++) {
        GameResult result = simulateGame(gen, gapSize, gapY, agentFunction, 10000);
        totalFitness += result.fitness();
    }
    
    return totalFitness / gamesPerEvaluation;
}

// Tournament selection: pick random agents, return index of best
int Evolution::tournamentSelect() {
    std::uniform_int_distribution<int> dist(0, populationSize - 1);
    
    int bestIndex = dist(gen);
    float bestFitness = fitness[bestIndex];
    
    // Pick tournamentSize - 1 more random agents and find best
    for (int i = 1; i < tournamentSize; i++) {
        int candidateIndex = dist(gen);
        if (fitness[candidateIndex] > bestFitness) {
            bestIndex = candidateIndex;
            bestFitness = fitness[candidateIndex];
        }
    }
    
    return bestIndex;
}

// Run one generation: evaluate, select, crossover, mutate
void Evolution::evolve() {
    // 1. Evaluate all agents
    for (int i = 0; i < populationSize; i++) {
        fitness[i] = evaluateAgent(population[i]);
    }
    
    // 2. Sort by fitness (best first)
    std::vector<int> indices(populationSize);
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(),
              [this](int a, int b) { return fitness[a] > fitness[b]; });
    
    // 3. Create new population
    std::vector<NeuralNetwork> newPopulation;
    newPopulation.reserve(populationSize);
    
    // 4. Elitism: keep top eliteRatio% unchanged
    int eliteSize = static_cast<int>(populationSize * eliteRatio);
    for (int i = 0; i < eliteSize; i++) {
        newPopulation.push_back(population[indices[i]]);
    }
    
    // 5. Fill rest with crossover and mutation
    while (static_cast<int>(newPopulation.size()) < populationSize) {
        // Select two parents via tournament selection
        int parent1Index = tournamentSelect();
        int parent2Index = tournamentSelect();
        
        // Ensure different parents
        while (parent2Index == parent1Index) {
            parent2Index = tournamentSelect();
        }
        
        // Crossover
        NeuralNetwork child = NeuralNetwork::crossover(
            population[parent1Index],
            population[parent2Index],
            gen
        );
        
        // Mutate
        child.mutate(mutationRate, mutationStrength, gen);
        
        newPopulation.push_back(child);
    }
    
    // 6. Replace old population
    population = newPopulation;
    
    // 7. Re-evaluate fitness for new population (for next generation)
    for (int i = 0; i < populationSize; i++) {
        fitness[i] = evaluateAgent(population[i]);
    }
}

// Get best agent
NeuralNetwork Evolution::getBestAgent() const {
    int bestIndex = 0;
    float bestFitness = fitness[0];
    
    for (int i = 1; i < populationSize; i++) {
        if (fitness[i] > bestFitness) {
            bestIndex = i;
            bestFitness = fitness[i];
        }
    }
    
    return population[bestIndex];
}

// Get best fitness
float Evolution::getBestFitness() const {
    return *std::max_element(fitness.begin(), fitness.end());
}

// Get average fitness
float Evolution::getAverageFitness() const {
    float sum = std::accumulate(fitness.begin(), fitness.end(), 0.0f);
    return sum / populationSize;
}

// Get statistics
void Evolution::getStatistics(float& best, float& average, float& worst) const {
    best = *std::max_element(fitness.begin(), fitness.end());
    worst = *std::min_element(fitness.begin(), fitness.end());
    average = std::accumulate(fitness.begin(), fitness.end(), 0.0f) / populationSize;
}

