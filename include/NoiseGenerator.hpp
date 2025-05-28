#pragma once
#include <vector>
#include <random>

class NoiseGenerator {
public:
    NoiseGenerator(int seed = 1);
    
    // Generate 2D Perlin noise
    float noise(float x, float y) const;
    
    // Generate Fractal Brownian Motion noise
    float fbm(float x, float y, int octaves, float persistence) const;
    
    // Set new seed and regenerate permutation table
    void setSeed(int seed);
    
    // Get current seed
    int getSeed() const { return seed; }

private:
    int seed;
    std::vector<int> p; // Permutation table
    
    // Helper functions
    float fade(float t) const;
    float lerp(float t, float a, float b) const;
    float grad(int hash, float x, float y) const;
    
    // Initialize permutation table
    void initPermutationTable();
}; 