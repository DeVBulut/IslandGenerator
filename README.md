# Procedural Island Generator

A C++ application that generates procedural islands using noise algorithms and allows real-time parameter adjustment. Built with SFML and Dear ImGui.

## Features

- **Real-time Island Generation**: Instantly see changes as you adjust parameters
- **Interactive GUI**: User-friendly interface with adjustable windows
- **Customizable Parameters**:
  - Noise Parameters:
    - Scale (1.0 - 10.0)
    - Octaves (1 - 8)
    - Persistence (0.1 - 1.0)
    - Seed (any integer)
  - Terrain Parameters:
    - Sea Level (0.0 - 1.0)
    - Beach Size (0.01 - 0.1)
    - Mountain Level (0.5 - 0.9)
    - Snow Level (0.7 - 1.0)
- **Export Functionality**: Save your generated islands as PNG files

## Dependencies

- SFML 2.5.1
- Dear ImGui
- OpenGL
- CMake (for building)
- C++17 compatible compiler

## Building the Project

1. Make sure you have all dependencies installed
2. Clone the repository
3. Create a build directory:
```bash
mkdir build
cd build
```
4. Generate build files with CMake:
```bash
cmake ..
```
5. Build the project:
```bash
cmake --build .
```

## Usage

1. Run the executable `ProceduralIslandGenerator.exe`
2. The interface consists of two main windows:
   - **Island Controls**: Adjust generation parameters
   - **Island Map**: Displays the generated island (can be moved and resized)

### Adjusting Parameters

- All parameters update the island in real-time
- The island will automatically regenerate when any parameter is changed
- Experiment with different combinations to create unique islands

### Exporting Islands

1. Click "Select Directory..." to choose where to save your island
2. Click "Export Now" to save the current island as a PNG file
3. The filename will include the seed number and timestamp

## Controls

- **Left Mouse Button**: Drag windows
- **Left Mouse Button + Window Edge**: Resize windows
- **Mouse Wheel**: Adjust slider values

## Tips

- Higher octave values create more detailed terrain
- Adjust the sea level to control how much of the island is visible
- Beach size affects the transition between water and land
- Mountain and snow levels control the elevation appearance
- Use different seeds to generate completely different islands
- The scale parameter affects the overall size of terrain features

## License

This project is open source and available under the MIT License.

## Contributing

Feel free to submit issues, fork the repository, and create pull requests for any improvements. 