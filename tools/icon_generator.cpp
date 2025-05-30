#include <SFML/Graphics.hpp>
#include <cmath>

// Function to create a circular gradient
sf::Color getGradientColor(float x, float y, float centerX, float centerY) {
    float dx = x - centerX;
    float dy = y - centerY;
    float distance = std::sqrt(dx * dx + dy * dy);
    float maxDistance = std::sqrt(2.0f) * 0.5f;
    float t = std::min(1.0f, distance / maxDistance);
    
    // Island colors
    sf::Color land(34, 139, 34);    // Forest green
    sf::Color water(0, 0, 139);     // Dark blue
    sf::Color beach(238, 214, 175); // Sandy color
    
    if (t < 0.4f) {
        return land;
    } else if (t < 0.45f) {
        float beachT = (t - 0.4f) / 0.05f;
        return sf::Color(
            land.r + (beach.r - land.r) * beachT,
            land.g + (beach.g - land.g) * beachT,
            land.b + (beach.b - land.b) * beachT
        );
    } else if (t < 0.5f) {
        float waterT = (t - 0.45f) / 0.05f;
        return sf::Color(
            beach.r + (water.r - beach.r) * waterT,
            beach.g + (water.g - beach.g) * waterT,
            beach.b + (water.b - beach.b) * waterT
        );
    }
    return water;
}

int main() {
    // Create icon sizes required for Windows
    const std::vector<unsigned int> iconSizes = {16, 32, 48, 256};
    std::vector<sf::Image> icons;
    
    for (unsigned int size : iconSizes) {
        sf::RenderTexture rt;
        rt.create(size, size);
        
        // Clear with transparent background
        rt.clear(sf::Color::Transparent);
        
        // Create the island shape
        sf::Image img;
        img.create(size, size, sf::Color::Transparent);
        
        float centerX = size * 0.5f;
        float centerY = size * 0.5f;
        
        // Generate the gradient
        for (unsigned int y = 0; y < size; y++) {
            for (unsigned int x = 0; x < size; x++) {
                float nx = x / float(size);
                float ny = y / float(size);
                sf::Color color = getGradientColor(nx, ny, 0.5f, 0.5f);
                img.setPixel(x, y, color);
            }
        }
        
        // Convert to texture and draw
        sf::Texture tex;
        tex.loadFromImage(img);
        
        sf::Sprite sprite(tex);
        rt.draw(sprite);
        rt.display();
        
        // Store the image
        icons.push_back(rt.getTexture().copyToImage());
    }
    
    // Save as ICO file
    if (!icons[0].saveToFile("resources/icon.ico")) {
        return 1;
    }
    
    return 0;
} 