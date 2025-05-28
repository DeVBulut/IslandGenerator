#include "IslandGenerator.hpp"
#include <cmath>

IslandGenerator::IslandGenerator(int width, int height) : width(width), height(height) {
}

void IslandGenerator::generate(const NoiseGenerator& noiseGen, float scale, int octaves, float persistence) {
    sf::Image image;
    image.create(width, height);
    
    // Generate gradient for island shape (higher in center, lower at edges)
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Get base noise value
            float nx = static_cast<float>(x) / width * scale;
            float ny = static_cast<float>(y) / height * scale;
            float noiseValue = noiseGen.fbm(nx, ny, octaves, persistence);
            
            // Apply radial gradient to create island shape
            float centerX = width * 0.5f;
            float centerY = height * 0.5f;
            float dx = x - centerX;
            float dy = y - centerY;
            float distanceFromCenter = std::sqrt(dx * dx + dy * dy) / (width * 0.5f);
            float gradient = 1.0f - std::pow(distanceFromCenter, 2.0f);
            
            // Combine noise with gradient
            float finalHeight = (noiseValue + 1.0f) * 0.5f * gradient;
            
            // Determine terrain type and color
            TerrainType terrain = getTerrainType(finalHeight);
            sf::Color color = getTerrainColor(terrain);
            
            image.setPixel(x, y, color);
        }
    }
    
    texture.loadFromImage(image);
}

IslandGenerator::TerrainType IslandGenerator::getTerrainType(float height) const {
    if (height < seaLevel - beachSize) {
        return TerrainType::DeepWater;
    } else if (height < seaLevel) {
        return TerrainType::ShallowWater;
    } else if (height < seaLevel + beachSize) {
        return TerrainType::Beach;
    } else if (height < mountainLevel) {
        return height < (mountainLevel + seaLevel) * 0.5f ? TerrainType::Grass : TerrainType::Forest;
    } else if (height < snowLevel) {
        return TerrainType::Mountain;
    } else {
        return TerrainType::Snow;
    }
}

sf::Color IslandGenerator::getTerrainColor(TerrainType terrain) const {
    switch (terrain) {
        case TerrainType::DeepWater:
            return sf::Color(0, 0, 150);
        case TerrainType::ShallowWater:
            return sf::Color(0, 0, 255);
        case TerrainType::Beach:
            return sf::Color(255, 255, 150);
        case TerrainType::Grass:
            return sf::Color(50, 180, 50);
        case TerrainType::Forest:
            return sf::Color(0, 100, 0);
        case TerrainType::Mountain:
            return sf::Color(120, 120, 120);
        case TerrainType::Snow:
            return sf::Color(255, 255, 255);
        default:
            return sf::Color::Black;
    }
}

bool IslandGenerator::exportToPNG(const std::string& filename) const {
    return texture.copyToImage().saveToFile(filename);
} 