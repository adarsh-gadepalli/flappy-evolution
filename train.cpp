#include "evolution.h"
#include "neural_network.h"
#include "simulation.h"
#include <iostream>
#include <iomanip>
#include <random>
#include <chrono>
#include <string>

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options]\n";
    std::cout << "Options:\n";
    std::cout << "  -p, --population SIZE     Population size (default: 50)\n";
    std::cout << "  -g, --generations NUM     Number of generations (default: 100)\n";
    std::cout << "  -e, --evaluations NUM     Games per evaluation (default: 5)\n";
    std::cout << "  -m, --mutation-rate RATE  Mutation rate (default: 0.1)\n";
    std::cout << "  -s, --mutation-strength STR Mutation strength (default: 0.1)\n";
    std::cout << "  -r, --elite-ratio RATIO   Elite ratio (default: 0.2)\n";
    std::cout << "  -t, --tournament-size NUM Tournament size (default: 3)\n";
    std::cout << "  -o, --output FILE         Output file for best agent (optional)\n";
    std::cout << "  -h, --help                Show this help message\n";
}

int main(int argc, char* argv[]) {
    // Default parameters
    int populationSize = 50;
    int numGenerations = 100;
    int gamesPerEvaluation = 5;
    float mutationRate = 0.1f;
    float mutationStrength = 0.1f;
    float eliteRatio = 0.2f;
    int tournamentSize = 3;
    std::string outputFile = "";
    
    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-p" || arg == "--population") {
            if (i + 1 < argc) {
                populationSize = std::stoi(argv[++i]);
            }
        } else if (arg == "-g" || arg == "--generations") {
            if (i + 1 < argc) {
                numGenerations = std::stoi(argv[++i]);
            }
        } else if (arg == "-e" || arg == "--evaluations") {
            if (i + 1 < argc) {
                gamesPerEvaluation = std::stoi(argv[++i]);
            }
        } else if (arg == "-m" || arg == "--mutation-rate") {
            if (i + 1 < argc) {
                mutationRate = std::stof(argv[++i]);
            }
        } else if (arg == "-s" || arg == "--mutation-strength") {
            if (i + 1 < argc) {
                mutationStrength = std::stof(argv[++i]);
            }
        } else if (arg == "-r" || arg == "--elite-ratio") {
            if (i + 1 < argc) {
                eliteRatio = std::stof(argv[++i]);
            }
        } else if (arg == "-t" || arg == "--tournament-size") {
            if (i + 1 < argc) {
                tournamentSize = std::stoi(argv[++i]);
            }
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            }
        }
    }
    
    // Initialize random number generators
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> gapSize(150.0f, 250.0f);
    std::uniform_real_distribution<float> gapY(200.0f, WINDOW_HEIGHT - 250.0f);
    
    // Network topology: 5 inputs → 8 hidden → 4 hidden → 1 output
    std::vector<int> topology = {5, 8, 4, 1};
    
    // Print configuration
    std::cout << "=== Evolutionary Flappy Bird Training ===\n\n";
    std::cout << "Configuration:\n";
    std::cout << "  Population size: " << populationSize << "\n";
    std::cout << "  Generations: " << numGenerations << "\n";
    std::cout << "  Games per evaluation: " << gamesPerEvaluation << "\n";
    std::cout << "  Mutation rate: " << mutationRate << "\n";
    std::cout << "  Mutation strength: " << mutationStrength << "\n";
    std::cout << "  Elite ratio: " << eliteRatio << "\n";
    std::cout << "  Tournament size: " << tournamentSize << "\n";
    std::cout << "  Network topology: ";
    for (size_t i = 0; i < topology.size(); i++) {
        std::cout << topology[i];
        if (i < topology.size() - 1) std::cout << " → ";
    }
    std::cout << "\n\n";
    
    // Create evolution object
    Evolution evolution(populationSize, topology, gamesPerEvaluation,
                       mutationRate, mutationStrength, eliteRatio, tournamentSize,
                       gen, gapSize, gapY);
    
    // Training loop
    float bestFitnessEver = 0.0f;
    int bestGeneration = 0;
    
    std::cout << "Starting training...\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\nGeneration | Best Fitness | Avg Fitness | Worst Fitness | Time (s)\n";
    std::cout << "-----------|--------------|-------------|---------------|----------\n";
    
    auto startTime = std::chrono::steady_clock::now();
    
    for (int generation = 0; generation < numGenerations; generation++) {
        auto genStartTime = std::chrono::steady_clock::now();
        
        // Evolve one generation
        evolution.evolve();
        
        // Get statistics
        float best, average, worst;
        evolution.getStatistics(best, average, worst);
        
        // Track best ever
        if (best > bestFitnessEver) {
            bestFitnessEver = best;
            bestGeneration = generation;
        }
        
        auto genEndTime = std::chrono::steady_clock::now();
        auto genDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
            genEndTime - genStartTime).count() / 1000.0;
        
        // Print statistics
        std::cout << std::setw(10) << generation << " | "
                  << std::setw(12) << best << " | "
                  << std::setw(11) << average << " | "
                  << std::setw(13) << worst << " | "
                  << std::setw(9) << std::setprecision(2) << genDuration << "\n";
        
        // Print progress every 10 generations
        if ((generation + 1) % 10 == 0) {
            std::cout << "\nProgress: " << (generation + 1) << "/" << numGenerations 
                      << " generations (" << std::setprecision(1) 
                      << (100.0 * (generation + 1) / numGenerations) << "%)\n";
            std::cout << "Best fitness so far: " << std::setprecision(2) << bestFitnessEver 
                      << " (generation " << bestGeneration << ")\n\n";
        }
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::seconds>(
        endTime - startTime).count();
    
    // Final statistics
    std::cout << "\n=== Training Complete ===\n";
    std::cout << "Total time: " << totalDuration << " seconds\n";
    std::cout << "Best fitness: " << std::setprecision(2) << bestFitnessEver 
              << " (generation " << bestGeneration << ")\n";
    
    float finalBest, finalAvg, finalWorst;
    evolution.getStatistics(finalBest, finalAvg, finalWorst);
    std::cout << "Final generation statistics:\n";
    std::cout << "  Best: " << finalBest << "\n";
    std::cout << "  Average: " << finalAvg << "\n";
    std::cout << "  Worst: " << finalWorst << "\n";
    
    // Get best agent
    NeuralNetwork bestAgent = evolution.getBestAgent();
    
    // Test best agent with a single game for demonstration
    std::cout << "\nTesting best agent...\n";
    auto agentFunction = [&bestAgent](const std::vector<float>& features) -> bool {
        float output = bestAgent.forward(features);
        return output > 0.5f;
    };
    
    GameResult testResult = simulateGame(gen, gapSize, gapY, agentFunction, 10000);
    std::cout << "Test game results:\n";
    std::cout << "  Score: " << testResult.score << "\n";
    std::cout << "  Frames alive: " << testResult.framesAlive << "\n";
    std::cout << "  Distance: " << std::setprecision(1) << testResult.distanceTraveled << "\n";
    std::cout << "  Fitness: " << std::setprecision(2) << testResult.fitness() << "\n";
    
    // Save best agent if output file specified
    if (!outputFile.empty()) {
        // For now, just print a message (full serialization would require more work)
        std::cout << "\nNote: Agent serialization not yet implemented.\n";
        std::cout << "Best agent topology: ";
        const auto& topo = bestAgent.getTopology();
        for (size_t i = 0; i < topo.size(); i++) {
            std::cout << topo[i];
            if (i < topo.size() - 1) std::cout << " → ";
        }
        std::cout << "\n";
    }
    
    return 0;
}

