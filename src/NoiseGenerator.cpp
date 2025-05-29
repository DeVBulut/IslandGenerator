#include "NoiseGenerator.hpp"
#include <cmath>

NoiseGenerator::NoiseGenerator() : seed(1) {}

float NoiseGenerator::noise(float x, float y) const {
    // Get integer coordinates
    int X = static_cast<int>(std::floor(x)) & 255;
    int Y = static_cast<int>(std::floor(y)) & 255;
    
    // Get decimal part
    x -= std::floor(x);
    y -= std::floor(y);
    
    // Compute fade curves
    float u = fade(x);
    float v = fade(y);
    
    // Hash coordinates of cube corners
    int A = hash(X, Y);
    int B = hash(X + 1, Y);
    int C = hash(X, Y + 1);
    int D = hash(X + 1, Y + 1);
    
    // Interpolate results
    float result = lerp(
        lerp(grad(A, x, y), grad(B, x - 1, y), u),
        lerp(grad(C, x, y - 1), grad(D, x - 1, y - 1), u),
        v
    );
    
    return result;
}

float NoiseGenerator::fbm(float x, float y, int octaves, float persistence) const {
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float maxValue = 0.0f;
    
    for (int i = 0; i < octaves; ++i) {
        total += noise(x * frequency, y * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.0f;
    }
    
    return total / maxValue;
}

void NoiseGenerator::setSeed(int newSeed) {
    seed = newSeed;
}

int NoiseGenerator::getSeed() const {
    return seed;
}

float NoiseGenerator::fade(float t) const {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float NoiseGenerator::lerp(float a, float b, float t) const {
    return a + t * (b - a);
}

float NoiseGenerator::grad(int hash, float x, float y) const {
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : h == 12 || h == 14 ? x : 0;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

int NoiseGenerator::hash(int x, int y) const {
    int hash = (seed * 1234567 + x * 2345678 + y * 3456789) & 255;
    hash = ((hash << 13) ^ hash) * (hash * (hash * hash * 15731 + 789221) + 1376312589);
    return hash & 255;
} 