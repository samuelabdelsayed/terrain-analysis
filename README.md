# Terrain Analysis Simulator

A comprehensive desktop application for professional terrain analysis and environmental simulation.

![Build Status](https://img.shields.io/badge/build-passing-brightgreen)
![Version](https://img.shields.io/badge/version-1.0.0-blue)
![License](https://img.shields.io/badge/license-MIT-green)

## Overview

The Terrain Analysis Simulator is a full-fledged desktop application designed for researchers, engineers, and analysts to conduct sophisticated terrain analysis and run realistic environmental simulations.

## Quick Start

### Prerequisites
- C++20 compatible compiler (GCC 10+, Clang 11+, MSVC 2019+)
- GLFW3 and GLM libraries

### Build Instructions

1. **Install dependencies (macOS)**
   ```bash
   brew install glfw glm
   ```

2. **Install dependencies (Ubuntu/Debian)**
   ```bash
   sudo apt-get install libglfw3-dev libglm-dev build-essential
   ```

3. **Build the application**
   ```bash
   chmod +x build.sh
   ./build.sh
   ```

4. **Run the simulator**
   ```bash
   cd build
   ./TerrainSimulator
   ```

## Controls

### Basic Navigation
- **TAB**: Toggle mouse capture for camera control
- **Arrow Keys**: Move camera around the terrain (←↑→↓)
- **Page Up/Down**: Adjust camera elevation
- **Mouse**: Look around (when captured with TAB)
- **ESC**: Exit application

### Simulation Controls
- **Space**: Start/pause unit simulation
- **R**: Reset simulation and generate new units
- **T**: Generate new terrain (when implemented)
- **C**: Reset camera to default position (when implemented)

## Features

### 🗺️ **Terrain Engine**
- **High-resolution 3D terrain rendering** (128x128 mesh)
- **Realistic elevation-based coloring** (water, grassland, mountains, snow)
- **Professional topographical contour mapping**
- **Military-grade contour line system** with multiple density levels
- **Interactive 3D navigation** with mouse and keyboard controls

### 🎯 **Simulation Engine**
- **Real-time entity simulation** with physics-based movement
- **Professional entity visualization** (rectangles for allied, diamonds for opposition)
- **Multi-unit coordination** and pathfinding
- **Dynamic scenario generation**

### 🤖 **AI System**
- **Intelligent entity behavior** with configurable complexity
- **Real-time decision making** and adaptation
- **Multi-agent coordination**

### � **Interactive Features**
- **Professional camera system** with smooth movement
- **Real-time rendering** with OpenGL graphics
- **Comprehensive input handling** for all controls
- **Visual feedback** and status reporting

## Technical Architecture

### Core Components
- **Application Core**: Main application loop and initialization
- **Terrain Engine**: 3D terrain generation and rendering
- **Simulation Engine**: Entity management and physics
- **Camera System**: 3D navigation and view control
- **Entity Symbols**: Professional military-style visualization
- **AI System**: Intelligent behavior and decision making
- **Database Manager**: Scenario storage and management

### Technology Stack
- **Language**: C++20 with modern standards
- **Graphics**: OpenGL with GLFW for windowing
- **Mathematics**: GLM for 3D vector/matrix operations
- **Build System**: Custom shell scripts for macOS
- **Architecture**: Modular design with clean separation of concerns

## Development

### Project Structure
```
terrain-simulator/
├── include/           # Header files
│   ├── ai/           # AI system headers
│   ├── core/         # Core application headers
│   ├── data/         # Database management headers
│   ├── graphics/     # Rendering and camera headers
│   ├── simulation/   # Simulation engine headers
│   └── terrain/      # Terrain generation headers
├── src/              # Source files
│   ├── ai/           # AI implementation
│   ├── core/         # Core application logic
│   ├── data/         # Database implementation
│   ├── graphics/     # Rendering implementation
│   ├── simulation/   # Simulation logic
│   └── terrain/      # Terrain generation
├── build.sh          # Build script
└── README.md         # This file
```

### Building from Source
The application uses a custom build system optimized for macOS development:

1. Ensure you have the required dependencies installed
2. Make the build script executable: `chmod +x build.sh`
3. Run the build script: `./build.sh`
4. The compiled application will be in the `build/` directory

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## Future Enhancements

- Real terrain data import (DEM, GeoTIFF, shapefile support)
- Advanced pathfinding algorithms
- Multi-threaded simulation processing
- Enhanced AI behaviors and strategies
- Network multiplayer support
- Scenario editor and scripting system
