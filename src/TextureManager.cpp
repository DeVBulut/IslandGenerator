#include "TextureManager.hpp"
#include "NoiseGenerator.hpp"
#include <SFML/Graphics.hpp>
#include <string>

TextureManager::TextureManager(int width, int height) : width(width), height(height) {
    renderTexture.create(width, height);
}

void TextureManager::generateNoiseTexture(const NoiseGenerator& noiseGen, float scale, int octaves, float persistence) {
    sf::Image image;
    image.create(width, height);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float nx = static_cast<float>(x) / width * scale;
            float ny = static_cast<float>(y) / height * scale;
            
            float value = noiseGen.fbm(nx, ny, octaves, persistence);
            image.setPixel(x, y, noiseToColor(value));
        }
    }
    
    texture.loadFromImage(image);
}

sf::Color TextureManager::noiseToColor(float value) const {
    unsigned char intensity = static_cast<unsigned char>((value * 0.5f + 0.5f) * 255);
    return sf::Color(intensity, intensity, intensity, 255);
}

bool TextureManager::exportToPNG(const std::string& filename) const {
    return texture.copyToImage().saveToFile(filename);
} 