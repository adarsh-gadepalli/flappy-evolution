#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

#include <vector>
#include <random>

class NeuralNetwork {
private:
    std::vector<int> topology;  // e.g., {5, 8, 4, 1}
    std::vector<std::vector<std::vector<float>>> weights;  // [layer][neuron][weight]
    std::vector<std::vector<float>> biases;  // [layer][neuron]
    
    // Activation functions
    static float relu(float x);
    static float sigmoid(float x);
    
    // Weight initialization
    void initializeWeights(std::mt19937& gen);
    
public:
    // Constructor: takes topology (e.g., {5, 8, 4, 1})
    NeuralNetwork(const std::vector<int>& topology, std::mt19937& gen);
    
    // Copy constructor
    NeuralNetwork(const NeuralNetwork& other);
    
    // Forward propagation: returns output (0-1 range)
    float forward(const std::vector<float>& inputs);
    
    // Get all weights as flat vector (for mutation/crossover)
    std::vector<float> getWeights() const;
    
    // Set all weights from flat vector
    void setWeights(const std::vector<float>& weights);
    
    // Get number of weights (for flat vector size)
    int getNumWeights() const;
    
    // Mutate: add Gaussian noise to weights
    void mutate(float mutationRate, float mutationStrength, std::mt19937& gen);
    
    // Crossover: create child from two parents (uniform crossover)
    static NeuralNetwork crossover(const NeuralNetwork& parent1, 
                                   const NeuralNetwork& parent2, 
                                   std::mt19937& gen);
    
    // Get topology
    const std::vector<int>& getTopology() const { return topology; }
};

#endif
