#pragma once
#include <SFML/Graphics.hpp>
#include "NoiseGenerator.hpp"
#include <string>

class TextureManager {
public:
    TextureManager(int width, int height);
    
    // Generate a texture from noise values
    void generateNoiseTexture(const NoiseGenerator& noiseGen, float scale, int octaves, float persistence);
    
    // Get the generated texture
    const sf::Texture& getTexture() const { return texture; }
    
    // Export current texture to PNG
    bool exportToPNG(const std::string& filename) const;

private:
    int width;
    int height;
    sf::RenderTexture renderTexture;
    sf::Texture texture;
    
    // Convert noise value to grayscale color
    sf::Color noiseToColor(float value) const;
}; 