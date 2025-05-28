#include "NoiseGenerator.hpp"
#include <cmath>

NoiseGenerator::NoiseGenerator(int seed) : seed(seed) {
    initPermutationTable();
}

void NoiseGenerator::setSeed(int newSeed) {
    seed = newSeed;
    initPermutationTable();
}

void NoiseGenerator::initPermutationTable() {
    p.resize(512);
    std::mt19937 gen(seed);
    
    for (int i = 0; i < 256; i++) {
        p[i] = i;
    }
    
    for (int i = 255; i > 0; i--) {
        std::uniform_int_distribution<> dis(0, i);
        int j = dis(gen);
        std::swap(p[i], p[j]);
    }
    
    for (int i = 0; i < 256; i++) {
        p[256 + i] = p[i];
    }
}

float NoiseGenerator::fade(float t) const {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float NoiseGenerator::lerp(float t, float a, float b) const {
    return a + t * (b - a);
}

float NoiseGenerator::grad(int hash, float x, float y) const {
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : h == 12 || h == 14 ? x : 0;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float NoiseGenerator::noise(float x, float y) const {
    int X = static_cast<int>(std::floor(x)) & 255;
    int Y = static_cast<int>(std::floor(y)) & 255;
    
    x -= std::floor(x);
    y -= std::floor(y);
    
    float u = fade(x);
    float v = fade(y);
    
    int A = p[X] + Y;
    int B = p[X + 1] + Y;
    
    return lerp(v,
               lerp(u,
                    grad(p[A], x, y),
                    grad(p[B], x - 1, y)),
               lerp(u,
                    grad(p[A + 1], x, y - 1),
                    grad(p[B + 1], x - 1, y - 1)));
}

float NoiseGenerator::fbm(float x, float y, int octaves, float persistence) const {
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float maxValue = 0.0f;
    
    for (int i = 0; i < octaves; i++) {
        total += noise(x * frequency, y * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.0f;
    }
    
    return total / maxValue;
} 