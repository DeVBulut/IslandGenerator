#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <imgui.h>
#include <backends/imgui_impl_opengl3.hpp>
#include "NoiseGenerator.hpp"
#include "IslandGenerator.hpp"
#include <windows.h>
#include <shobjidl.h> 
#include <filesystem>
#include <shlobj.h>

// Global texture for ImGui font
sf::Texture* g_fontTexture = nullptr;

// ImGui SFML implementation
void ImGui_SFML_Init(sf::RenderWindow& window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    
    // Initialize display size
    io.DisplaySize = ImVec2(window.getSize().x, window.getSize().y);
    
    // Initialize OpenGL3 backend
    ImGui_ImplOpenGL3_Init();
    
    // Build font atlas
    unsigned char* pixels;
    int width, height;
    io.Fonts->AddFontDefault();
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    
    // Create and initialize font texture
    g_fontTexture = new sf::Texture();
    if (!g_fontTexture->create(width, height)) {
        delete g_fontTexture;
        g_fontTexture = nullptr;
        throw std::runtime_error("Failed to create font texture");
    }
    g_fontTexture->update(pixels);
    
    // Store texture in ImGui
    io.Fonts->TexID = (ImTextureID)(intptr_t)g_fontTexture->getNativeHandle();
    
    // Clear temporary pixel data
    io.Fonts->ClearTexData();
}

void ImGui_SFML_ProcessEvent(const sf::Event& event) {
    ImGuiIO& io = ImGui::GetIO();
    
    switch (event.type) {
        case sf::Event::MouseMoved:
            io.MousePos = ImVec2(event.mouseMove.x, event.mouseMove.y);
            break;
        case sf::Event::MouseButtonPressed:
        case sf::Event::MouseButtonReleased:
            io.MouseDown[0] = sf::Mouse::isButtonPressed(sf::Mouse::Left);
            io.MouseDown[1] = sf::Mouse::isButtonPressed(sf::Mouse::Right);
            io.MouseDown[2] = sf::Mouse::isButtonPressed(sf::Mouse::Middle);
            break;
        case sf::Event::MouseWheelScrolled:
            io.MouseWheel += event.mouseWheelScroll.delta;
            break;
        case sf::Event::KeyPressed:
        case sf::Event::KeyReleased: {
            ImGuiKey key = ImGuiKey_None;
            switch (event.key.code) {
                case sf::Keyboard::Tab: key = ImGuiKey_Tab; break;
                case sf::Keyboard::Left: key = ImGuiKey_LeftArrow; break;
                case sf::Keyboard::Right: key = ImGuiKey_RightArrow; break;
                case sf::Keyboard::Up: key = ImGuiKey_UpArrow; break;
                case sf::Keyboard::Down: key = ImGuiKey_DownArrow; break;
                case sf::Keyboard::PageUp: key = ImGuiKey_PageUp; break;
                case sf::Keyboard::PageDown: key = ImGuiKey_PageDown; break;
                case sf::Keyboard::Home: key = ImGuiKey_Home; break;
                case sf::Keyboard::End: key = ImGuiKey_End; break;
                case sf::Keyboard::Insert: key = ImGuiKey_Insert; break;
                case sf::Keyboard::Delete: key = ImGuiKey_Delete; break;
                case sf::Keyboard::BackSpace: key = ImGuiKey_Backspace; break;
                case sf::Keyboard::Space: key = ImGuiKey_Space; break;
                case sf::Keyboard::Return: key = ImGuiKey_Enter; break;
                case sf::Keyboard::Escape: key = ImGuiKey_Escape; break;
                default:
                    if (event.key.code >= sf::Keyboard::A && event.key.code <= sf::Keyboard::Z)
                        key = static_cast<ImGuiKey>(ImGuiKey_A + (event.key.code - sf::Keyboard::A));
                    break;
            }
            if (key != ImGuiKey_None)
                io.AddKeyEvent(key, event.type == sf::Event::KeyPressed);
            
            io.AddKeyEvent(ImGuiKey_ModCtrl, event.key.control);
            io.AddKeyEvent(ImGuiKey_ModShift, event.key.shift);
            io.AddKeyEvent(ImGuiKey_ModAlt, event.key.alt);
            break;
        }
        case sf::Event::TextEntered:
            if (event.text.unicode > 0 && event.text.unicode < 0x10000)
                io.AddInputCharacter(event.text.unicode);
            break;
        default:
            break;
    }
}

void ImGui_SFML_Update(sf::RenderWindow& window, sf::Time dt) {
    ImGuiIO& io = ImGui::GetIO();
    io.DeltaTime = dt.asSeconds();
    io.DisplaySize = ImVec2(window.getSize().x, window.getSize().y);
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
}

void ImGui_SFML_Render(sf::RenderWindow& window) {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGui_SFML_Shutdown() {
    if (g_fontTexture) {
        delete g_fontTexture;
        g_fontTexture = nullptr;
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}

// Function to get screen resolution
sf::Vector2u GetScreenResolution() {
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    return sf::Vector2u(desktop.width, desktop.height);
}

// Function to calculate UI scale based on window size
float CalculateUIScale(const sf::RenderWindow& window) {
    float baseWidth = 1280.0f; // Base window width for scaling
    float currentWidth = static_cast<float>(window.getSize().x);
    return std::max(0.8f, std::min(2.0f, currentWidth / baseWidth));
}

// Function to show folder selection dialog
std::string ShowFolderDialog() {
    std::string folderPath;
    
    // Initialize COM
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (SUCCEEDED(hr)) {
        IFileOpenDialog *pFileOpen;
        
        // Create the FileOpenDialog object
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, 
                            IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
        
        if (SUCCEEDED(hr)) {
            // Set options to pick folders
            DWORD dwOptions;
            pFileOpen->GetOptions(&dwOptions);
            pFileOpen->SetOptions(dwOptions | FOS_PICKFOLDERS);
            
            // Show the Open dialog
            hr = pFileOpen->Show(NULL);
            
            if (SUCCEEDED(hr)) {
                IShellItem *pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr)) {
                    PWSTR pszFolderPath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolderPath);
                    
                    if (SUCCEEDED(hr)) {
                        std::wstring ws(pszFolderPath);
                        folderPath = std::string(ws.begin(), ws.end());
                        CoTaskMemFree(pszFolderPath);
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }
    
    return folderPath;
}

int main() {
    // Get screen resolution
    sf::Vector2u screenSize = GetScreenResolution();
    
    // Calculate window size (80% of screen size)
    unsigned int windowWidth = static_cast<unsigned int>(screenSize.x * 0.8f);
    unsigned int windowHeight = static_cast<unsigned int>(screenSize.y * 0.8f);
    
    // Create window
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Procedural Island Generator");
    window.setFramerateLimit(60);
    
    // Initialize ImGui
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    
    // Load fonts
    float baseFontSize = 16.0f;
    float uiScale = CalculateUIScale(window);
    io.Fonts->Clear();
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", baseFontSize * uiScale);
    
    ImGui_SFML_Init(window);
    
    // Style settings
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(uiScale);
    
    // Create generators
    NoiseGenerator noiseGen;
    IslandGenerator islandGen(512, 512);
    
    // Parameters
    float scale = 4.0f;
    int octaves = 6;
    float persistence = 0.5f;
    int seed = 1;
    
    // Terrain parameters
    float seaLevel = 0.4f;
    float beachSize = 0.05f;
    float mountainLevel = 0.7f;
    float snowLevel = 0.85f;
    
    // Export parameters
    static std::string selectedExportPath;
    
    // Status message
    std::string statusMessage = "Welcome to Island Generator! Adjust parameters to generate your island.";
    float statusMessageTimer = 5.0f;
    
    // Generate initial island
    islandGen.generate(noiseGen, scale, octaves, persistence);
    
    // Create sprite for displaying the texture
    sf::Sprite displaySprite(islandGen.getTexture());
    
    // Initial window positions and sizes
    ImVec2 controlsPos(20, 20);
    ImVec2 controlsSize(350, 400);
    ImVec2 mapPos(400, 20);
    ImVec2 mapSize(512, 512);
    
    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui_SFML_ProcessEvent(event);
            
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::Resized) {
                // Update viewport
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
                
                // Recalculate UI scale
                uiScale = CalculateUIScale(window);
                
                // Reload fonts with new scale
                io.Fonts->Clear();
                io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", baseFontSize * uiScale);
                ImGui_SFML_Init(window); // Reinitialize with new font
                
                // Update style scaling
                style.ScaleAllSizes(uiScale);
            }
        }
        
        ImGui_SFML_Update(window, deltaClock.restart());
        
        // Push the scaled font as default
        ImGui::PushFont(io.Fonts->Fonts[0]);
        
        // Update status message timer
        if (statusMessageTimer > 0.0f) {
            statusMessageTimer -= deltaClock.getElapsedTime().asSeconds();
            if (statusMessageTimer <= 0.0f) {
                statusMessage.clear();
            }
        }
        
        // Controls window
        ImGui::SetNextWindowPos(controlsPos, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(controlsSize, ImGuiCond_FirstUseEver);
        
        ImGui::Begin("Island Controls");
        
        bool regenerate = false;
        
        if (ImGui::CollapsingHeader("Noise Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::SliderFloat("Scale", &scale, 1.0f, 10.0f)) regenerate = true;
            if (ImGui::SliderInt("Octaves", &octaves, 1, 8)) regenerate = true;
            if (ImGui::SliderFloat("Persistence", &persistence, 0.1f, 1.0f)) regenerate = true;
            if (ImGui::InputInt("Seed", &seed)) {
                noiseGen.setSeed(seed);
                regenerate = true;
            }
        }
        
        if (ImGui::CollapsingHeader("Terrain Parameters", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::SliderFloat("Sea Level", &seaLevel, 0.0f, 1.0f)) {
                islandGen.setSeaLevel(seaLevel);
                regenerate = true;
            }
            if (ImGui::SliderFloat("Beach Size", &beachSize, 0.01f, 0.1f)) {
                islandGen.setBeachSize(beachSize);
                regenerate = true;
            }
            if (ImGui::SliderFloat("Mountain Level", &mountainLevel, 0.5f, 0.9f)) {
                islandGen.setMountainLevel(mountainLevel);
                regenerate = true;
            }
            if (ImGui::SliderFloat("Snow Level", &snowLevel, 0.7f, 1.0f)) {
                islandGen.setSnowLevel(snowLevel);
                regenerate = true;
            }
        }
        
        // Regenerate if any parameter changed
        if (regenerate) {
            try {
                islandGen.generate(noiseGen, scale, octaves, persistence);
                displaySprite.setTexture(islandGen.getTexture(), true);
                statusMessage = "Island updated with new parameters!";
                statusMessageTimer = 2.0f;
            } catch (const std::exception& e) {
                statusMessage = "Error generating island: " + std::string(e.what());
                statusMessageTimer = 5.0f;
            }
        }
        
        ImGui::Separator();
        
        // Export section
        if (ImGui::CollapsingHeader("Export Options", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Show current selected directory
            ImGui::Text("Selected export directory:");
            if (selectedExportPath.empty()) {
                ImGui::TextWrapped("No directory selected");
            } else {
                ImGui::TextWrapped("%s", selectedExportPath.c_str());
            }
            
            // Select Directory button
            if (ImGui::Button("Select Directory...")) {
                try {
                    std::string folderPath = ShowFolderDialog();
                    if (!folderPath.empty()) {
                        selectedExportPath = folderPath;
                        statusMessage = "Export directory selected: " + folderPath;
                        statusMessageTimer = 5.0f;
                    }
                } catch (const std::exception& e) {
                    statusMessage = "Error selecting directory: " + std::string(e.what());
                    statusMessageTimer = 5.0f;
                }
            }
            
            ImGui::SameLine();
            
            // Export button (only enabled if directory is selected)
            if (ImGui::Button("Export Now", ImVec2(120, 0))) {
                if (selectedExportPath.empty()) {
                    statusMessage = "Please select an export directory first!";
                    statusMessageTimer = 5.0f;
                } else {
                    try {
                        // Create filename with seed and timestamp
                        auto now = std::chrono::system_clock::now();
                        auto timestamp = std::chrono::system_clock::to_time_t(now);
                        std::string filename = "island_seed" + std::to_string(seed) + "_" + 
                                             std::to_string(timestamp) + ".png";
                        
                        // Combine folder path and filename
                        std::string fullPath = selectedExportPath + "\\" + filename;
                        
                        islandGen.exportToPNG(fullPath);
                        statusMessage = "Island exported successfully!\nLocation: " + fullPath;
                        statusMessageTimer = 8.0f;
                    } catch (const std::exception& e) {
                        statusMessage = "Error exporting PNG: " + std::string(e.what());
                        statusMessageTimer = 5.0f;
                    }
                }
            }
            
            ImGui::TextWrapped("First select a directory, then click Export Now to save your island image.");
        }
        
        // Display status message if any
        if (!statusMessage.empty()) {
            ImGui::Separator();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(50, 255, 50, 255));
            ImGui::TextWrapped("%s", statusMessage.c_str());
            ImGui::PopStyleColor();
        }
        
        ImGui::End();
        
        // Map window
        ImGui::SetNextWindowPos(mapPos, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(mapSize.x * uiScale, mapSize.y * uiScale), ImGuiCond_FirstUseEver);
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        
        ImGui::Begin("Island Map", nullptr, 
            ImGuiWindowFlags_NoScrollbar | 
            ImGuiWindowFlags_NoScrollWithMouse);
        
        // Get the content region and window position
        ImVec2 contentSize = ImGui::GetContentRegionAvail();
        ImVec2 windowPos = ImGui::GetCursorScreenPos();
        
        // Calculate scaling to fit the window while maintaining aspect ratio
        float scaleX = contentSize.x / displaySprite.getTexture()->getSize().x;
        float scaleY = contentSize.y / displaySprite.getTexture()->getSize().y;
        float scale = std::min(scaleX, scaleY);
        
        // Calculate centered position
        float spriteWidth = displaySprite.getTexture()->getSize().x * scale;
        float spriteHeight = displaySprite.getTexture()->getSize().y * scale;
        float offsetX = (contentSize.x - spriteWidth) * 0.5f;
        float offsetY = (contentSize.y - spriteHeight) * 0.5f;
        
        // Draw the texture in the ImGui window
        ImTextureID texId = (ImTextureID)(intptr_t)displaySprite.getTexture()->getNativeHandle();
        ImGui::GetWindowDrawList()->AddImage(
            texId,
            ImVec2(windowPos.x + offsetX, windowPos.y + offsetY),
            ImVec2(windowPos.x + offsetX + spriteWidth, windowPos.y + offsetY + spriteHeight)
        );
        
        ImGui::End();
        ImGui::PopStyleVar();
        
        // Pop the font before rendering
        ImGui::PopFont();
        
        // Render
        window.clear(sf::Color(100, 100, 100));
        ImGui_SFML_Render(window);
        window.display();
    }
    
    ImGui_SFML_Shutdown();
    return 0;
} 