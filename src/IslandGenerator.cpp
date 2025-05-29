#include "IslandGenerator.hpp"
#include <cmath>

IslandGenerator::IslandGenerator(unsigned int width, unsigned int height)
    : width(width)
    , height(height)
    , seaLevel(0.500f)
    , beachSize(0.030f)
    , mountainLevel(0.610f)
    , snowLevel(0.700f)
{
    if (!renderTexture.create(width, height)) {
        throw std::runtime_error("Failed to create render texture");
    }
}

void IslandGenerator::generate(const NoiseGenerator& noiseGen, float scale, int octaves, float persistence) {
    sf::Image image;
    image.create(width, height, sf::Color::Black);
    
    // Define multiple island centers with better distribution
    const int numCenters = 5;
    struct IslandCenter {
        float x, y;
        float influence;
        float size;
    };
    
    IslandCenter centers[numCenters] = {
        {0.5f, 0.5f, 0.9f, 1.0f},      // Main island
        {0.25f, 0.3f, 0.7f, 0.8f},     // Left island
        {0.75f, 0.4f, 0.6f, 0.7f},     // Right island
        {0.35f, 0.7f, 0.5f, 0.6f},     // Bottom-left island
        {0.65f, 0.65f, 0.4f, 0.5f}     // Top-right island
    };
    
    // Generate noise values and apply colors
    for (unsigned int y = 0; y < height; ++y) {
        for (unsigned int x = 0; x < width; ++x) {
            // Calculate normalized coordinates
            float nx = static_cast<float>(x) / width;
            float ny = static_cast<float>(y) / height;
            
            // Get base noise value first
            float noiseValue = noiseGen.fbm(nx * scale, ny * scale, octaves, persistence);
            noiseValue = (noiseValue + 1.0f) * 0.5f; // Normalize to [0,1]
            
            // Calculate combined gradient from all island centers
            float maxGradient = 0.0f;
            for (const auto& center : centers) {
                float dx = nx - center.x;
                float dy = ny - center.y;
                float distanceFromCenter = std::sqrt(dx * dx + dy * dy) / center.size;
                
                // Smoother falloff using cubic function
                float gradient = 0.0f;
                if (distanceFromCenter < 1.0f) {
                    gradient = 1.0f - (3.0f * std::pow(distanceFromCenter, 2.0f) - 2.0f * std::pow(distanceFromCenter, 3.0f));
                    gradient *= center.influence;
                }
                
                maxGradient = std::max(maxGradient, gradient);
            }
            
            // Combine noise and gradient with better blending
            float finalHeight = noiseValue * maxGradient;
            
            // Add some variation to water depth
            if (finalHeight < seaLevel) {
                finalHeight *= 0.8f + 0.2f * noiseValue;
            }
            
            // Set pixel color directly
            image.setPixel(x, y, getTerrainColor(finalHeight));
        }
    }
    
    // Update the render texture with the generated image
    renderTexture.clear();
    sf::Texture tempTexture;
    tempTexture.loadFromImage(image);
    sf::Sprite sprite(tempTexture);
    renderTexture.draw(sprite);
    renderTexture.display();
}

const sf::Texture& IslandGenerator::getTexture() const {
    return renderTexture.getTexture();
}

void IslandGenerator::exportToPNG(const std::string& filename) const {
    if (!renderTexture.getTexture().copyToImage().saveToFile(filename)) {
        throw std::runtime_error("Failed to save image to file: " + filename);
    }
}

void IslandGenerator::setSeaLevel(float level) {
    seaLevel = level;
}

void IslandGenerator::setBeachSize(float size) {
    beachSize = size;
}

void IslandGenerator::setMountainLevel(float level) {
    mountainLevel = level;
}

void IslandGenerator::setSnowLevel(float level) {
    snowLevel = level;
}

sf::Color IslandGenerator::getTerrainColor(float height) const {
    if (height < seaLevel - beachSize) {
        // Deep water
        return sf::Color(0, 0, 139); // Dark blue
    }
    else if (height < seaLevel) {
        // Shallow water
        return sf::Color(0, 191, 255); // Deep sky blue
    }
    else if (height < seaLevel + beachSize) {
        // Beach
        return sf::Color(238, 214, 175); // Sandy
    }
    else if (height < mountainLevel) {
        // Grass/forest
        float t = (height - (seaLevel + beachSize)) / (mountainLevel - (seaLevel + beachSize));
        return sf::Color(
            34 + static_cast<sf::Uint8>(t * (85 - 34)),
            139 + static_cast<sf::Uint8>(t * (107 - 139)),
            34 + static_cast<sf::Uint8>(t * (47 - 34))
        );
    }
    else if (height < snowLevel) {
        // Mountain
        return sf::Color(139, 137, 137); // Gray
    }
    else {
        // Snow
        return sf::Color(255, 250, 250); // Snow white
    }
} 