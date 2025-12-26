// Simple test program for NeuralNetwork
#include "neural_network.h"
#include <iostream>
#include <random>
#include <iomanip>

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Create network with topology: 5 inputs → 8 hidden → 4 hidden → 1 output
    std::vector<int> topology = {5, 8, 4, 1};
    NeuralNetwork network(topology, gen);
    
    std::cout << "Neural Network Test\n";
    std::cout << "===================\n\n";
    
    // Test 1: Check topology
    std::cout << "Topology: ";
    for (int size : topology) {
        std::cout << size << " → ";
    }
    std::cout << "\b\b\b   \n";
    std::cout << "Total weights: " << network.getNumWeights() << "\n\n";
    
    // Test 2: Forward propagation with sample inputs
    std::cout << "Test forward propagation:\n";
    std::vector<float> testInputs = {0.5f, 0.3f, 0.7f, 0.2f, 0.9f};
    std::cout << "Input: [";
    for (size_t i = 0; i < testInputs.size(); i++) {
        std::cout << testInputs[i];
        if (i < testInputs.size() - 1) std::cout << ", ";
    }
    std::cout << "]\n";
    
    float output = network.forward(testInputs);
    std::cout << "Output: " << std::fixed << std::setprecision(4) << output << "\n";
    std::cout << "Decision (flap if > 0.5): " << (output > 0.5f ? "FLAP" : "DON'T FLAP") << "\n\n";
    
    // Test 3: Test multiple random inputs
    std::cout << "Testing multiple random inputs:\n";
    std::uniform_real_distribution<float> inputDist(0.0f, 1.0f);
    for (int i = 0; i < 5; i++) {
        std::vector<float> randomInputs(5);
        for (float& val : randomInputs) {
            val = inputDist(gen);
        }
        float out = network.forward(randomInputs);
        std::cout << "  Input " << (i+1) << ": output = " << std::setprecision(4) << out;
        std::cout << " → " << (out > 0.5f ? "FLAP" : "DON'T FLAP") << "\n";
    }
    std::cout << "\n";
    
    // Test 4: Test mutation
    std::cout << "Testing mutation:\n";
    float outputBefore = network.forward(testInputs);
    network.mutate(0.1f, 0.05f, gen); // 10% mutation rate, 0.05 strength
    float outputAfter = network.forward(testInputs);
    std::cout << "  Output before mutation: " << std::setprecision(4) << outputBefore << "\n";
    std::cout << "  Output after mutation:  " << std::setprecision(4) << outputAfter << "\n";
    std::cout << "  Difference: " << std::setprecision(4) << (outputAfter - outputBefore) << "\n\n";
    
    // Test 5: Test crossover
    std::cout << "Testing crossover:\n";
    NeuralNetwork parent1(topology, gen);
    NeuralNetwork parent2(topology, gen);
    NeuralNetwork child = NeuralNetwork::crossover(parent1, parent2, gen);
    
    float parent1Out = parent1.forward(testInputs);
    float parent2Out = parent2.forward(testInputs);
    float childOut = child.forward(testInputs);
    
    std::cout << "  Parent 1 output: " << std::setprecision(4) << parent1Out << "\n";
    std::cout << "  Parent 2 output: " << std::setprecision(4) << parent2Out << "\n";
    std::cout << "  Child output:    " << std::setprecision(4) << childOut << "\n";
    std::cout << "  (Child should be mix of parents)\n";
    
    std::cout << "\nAll tests completed!\n";
    return 0;
}

