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

    IslandGenerator(int width, int height);
    
    // Generate island using current parameters
    void generate(const NoiseGenerator& noiseGen, float scale, int octaves, float persistence);
    
    // Get the generated texture
    const sf::Texture& getTexture() const { return texture; }
    
    // Export current texture to PNG
    bool exportToPNG(const std::string& filename) const;
    
    // Set parameters
    void setSeaLevel(float level) { seaLevel = level; }
    void setBeachSize(float size) { beachSize = size; }
    void setMountainLevel(float level) { mountainLevel = level; }
    void setSnowLevel(float level) { snowLevel = level; }

private:
    int width;
    int height;
    sf::Texture texture;
    
    // Terrain parameters
    float seaLevel = 0.4f;
    float beachSize = 0.05f;
    float mountainLevel = 0.7f;
    float snowLevel = 0.85f;
    
    // Helper functions
    TerrainType getTerrainType(float height) const;
    sf::Color getTerrainColor(TerrainType terrain) const;
    float getGradientNoise(float x, float y, const NoiseGenerator& noiseGen) const;
}; 