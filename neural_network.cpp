#include "neural_network.h"
#include <algorithm>
#include <cmath>
#include <numeric>

// ReLU activation function
float NeuralNetwork::relu(float x) {
    return std::max(0.0f, x);
}

// Sigmoid activation function
float NeuralNetwork::sigmoid(float x) {
    return 1.0f / (1.0f + std::exp(-x));
}

// Constructor: initialize network with given topology
NeuralNetwork::NeuralNetwork(const std::vector<int>& topology, std::mt19937& gen)
    : topology(topology) {
    
    // Initialize weights and biases
    weights.resize(topology.size() - 1);
    biases.resize(topology.size() - 1);
    
    for (size_t layer = 0; layer < topology.size() - 1; layer++) {
        int numNeurons = topology[layer + 1];
        int numInputs = topology[layer];
        
        weights[layer].resize(numNeurons);
        biases[layer].resize(numNeurons);
        
        // Xavier/Glorot initialization: weights ~ N(0, sqrt(2 / (fan_in + fan_out)))
        float stddev = std::sqrt(2.0f / (numInputs + numNeurons));
        std::normal_distribution<float> dist(0.0f, stddev);
        
        for (int neuron = 0; neuron < numNeurons; neuron++) {
            weights[layer][neuron].resize(numInputs);
            for (int input = 0; input < numInputs; input++) {
                weights[layer][neuron][input] = dist(gen);
            }
            biases[layer][neuron] = dist(gen);
        }
    }
}

// Copy constructor
NeuralNetwork::NeuralNetwork(const NeuralNetwork& other)
    : topology(other.topology), weights(other.weights), biases(other.biases) {
}

// Forward propagation
float NeuralNetwork::forward(const std::vector<float>& inputs) {
    if (inputs.size() != static_cast<size_t>(topology[0])) {
        return 0.0f; // Error: wrong input size
    }
    
    std::vector<float> current = inputs;
    
    // Propagate through each layer
    for (size_t layer = 0; layer < weights.size(); layer++) {
        std::vector<float> next(weights[layer].size());
        
        for (size_t neuron = 0; neuron < weights[layer].size(); neuron++) {
            // Calculate weighted sum
            float sum = biases[layer][neuron];
            for (size_t input = 0; input < current.size(); input++) {
                sum += weights[layer][neuron][input] * current[input];
            }
            
            // Apply activation function
            if (layer == weights.size() - 1) {
                // Last layer: sigmoid
                next[neuron] = sigmoid(sum);
            } else {
                // Hidden layers: ReLU
                next[neuron] = relu(sum);
            }
        }
        
        current = next;
    }
    
    // Return output (single value for our network)
    return current[0];
}

// Get all weights as flat vector
std::vector<float> NeuralNetwork::getWeights() const {
    std::vector<float> flat;
    
    // Add biases first, then weights
    for (size_t layer = 0; layer < biases.size(); layer++) {
        for (size_t neuron = 0; neuron < biases[layer].size(); neuron++) {
            flat.push_back(biases[layer][neuron]);
        }
    }
    
    for (size_t layer = 0; layer < weights.size(); layer++) {
        for (size_t neuron = 0; neuron < weights[layer].size(); neuron++) {
            for (size_t weight = 0; weight < weights[layer][neuron].size(); weight++) {
                flat.push_back(weights[layer][neuron][weight]);
            }
        }
    }
    
    return flat;
}

// Set all weights from flat vector
void NeuralNetwork::setWeights(const std::vector<float>& flat) {
    size_t index = 0;
    
    // Set biases first
    for (size_t layer = 0; layer < biases.size(); layer++) {
        for (size_t neuron = 0; neuron < biases[layer].size(); neuron++) {
            if (index < flat.size()) {
                biases[layer][neuron] = flat[index++];
            }
        }
    }
    
    // Set weights
    for (size_t layer = 0; layer < weights.size(); layer++) {
        for (size_t neuron = 0; neuron < weights[layer].size(); neuron++) {
            for (size_t weight = 0; weight < weights[layer][neuron].size(); weight++) {
                if (index < flat.size()) {
                    weights[layer][neuron][weight] = flat[index++];
                }
            }
        }
    }
}

// Get total number of weights (including biases)
int NeuralNetwork::getNumWeights() const {
    int count = 0;
    
    // Count biases
    for (const auto& layer : biases) {
        count += layer.size();
    }
    
    // Count weights
    for (const auto& layer : weights) {
        for (const auto& neuron : layer) {
            count += neuron.size();
        }
    }
    
    return count;
}

// Mutate: add Gaussian noise to random weights
void NeuralNetwork::mutate(float mutationRate, float mutationStrength, std::mt19937& gen) {
    std::uniform_real_distribution<float> probDist(0.0f, 1.0f);
    std::normal_distribution<float> noiseDist(0.0f, mutationStrength);
    
    // Mutate biases
    for (auto& layer : biases) {
        for (auto& bias : layer) {
            if (probDist(gen) < mutationRate) {
                bias += noiseDist(gen);
            }
        }
    }
    
    // Mutate weights
    for (auto& layer : weights) {
        for (auto& neuron : layer) {
            for (auto& weight : neuron) {
                if (probDist(gen) < mutationRate) {
                    weight += noiseDist(gen);
                }
            }
        }
    }
}

// Crossover: uniform crossover (randomly pick from each parent)
NeuralNetwork NeuralNetwork::crossover(const NeuralNetwork& parent1, 
                                        const NeuralNetwork& parent2, 
                                        std::mt19937& gen) {
    // Both parents must have same topology
    if (parent1.topology != parent2.topology) {
        return parent1; // Return first parent if mismatch
    }
    
    NeuralNetwork child(parent1.topology, gen);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    
    // Crossover biases
    for (size_t layer = 0; layer < child.biases.size(); layer++) {
        for (size_t neuron = 0; neuron < child.biases[layer].size(); neuron++) {
            child.biases[layer][neuron] = (dist(gen) < 0.5f) 
                ? parent1.biases[layer][neuron] 
                : parent2.biases[layer][neuron];
        }
    }
    
    // Crossover weights
    for (size_t layer = 0; layer < child.weights.size(); layer++) {
        for (size_t neuron = 0; neuron < child.weights[layer].size(); neuron++) {
            for (size_t weight = 0; weight < child.weights[layer][neuron].size(); weight++) {
                child.weights[layer][neuron][weight] = (dist(gen) < 0.5f)
                    ? parent1.weights[layer][neuron][weight]
                    : parent2.weights[layer][neuron][weight];
            }
        }
    }
    
    return child;
}

