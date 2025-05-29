#pragma once
#include <SFML/Graphics.hpp>
#include "NoiseGenerator.hpp"

class IslandGenerator {
public:
    // Terrain types
    enum class TerrainType {
        DeepWater,
        ShallowWater,
        Beach,
        Grass,
        Forest,
        Mountain,
        Snow
    };

    IslandGenerator(unsigned int width, unsigned int height);
    
    // Generate island using given noise parameters
    void generate(const NoiseGenerator& noiseGen, float scale, int octaves, float persistence);
    
    // Get the generated texture
    const sf::Texture& getTexture() const;
    
    // Export the generated island to a PNG file
    void exportToPNG(const std::string& filename) const;
    
    // Set terrain parameters
    void setSeaLevel(float level);
    void setBeachSize(float size);
    void setMountainLevel(float level);
    void setSnowLevel(float level);
    
private:
    unsigned int width;
    unsigned int height;
    sf::RenderTexture renderTexture;
    
    // Terrain parameters
    float seaLevel;
    float beachSize;
    float mountainLevel;
    float snowLevel;
    
    // Helper function to get terrain color based on height
    sf::Color getTerrainColor(float height) const;
}; 