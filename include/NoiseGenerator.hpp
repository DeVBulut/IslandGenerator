#pragma once

class NoiseGenerator {
public:
    NoiseGenerator();
    
    // Generate noise value at given coordinates
    float noise(float x, float y) const;
    
    // Generate Fractal Brownian Motion noise
    float fbm(float x, float y, int octaves, float persistence) const;
    
    // Set seed for noise generation
    void setSeed(int newSeed);
    
    // Get current seed
    int getSeed() const;

private:
    int seed;
    
    // Helper functions for noise generation
    float fade(float t) const;
    float lerp(float a, float b, float t) const;
    float grad(int hash, float x, float y) const;
    int hash(int x, int y) const;
}; 