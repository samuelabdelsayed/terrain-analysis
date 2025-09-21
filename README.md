# Terrain Simulator

A comprehensive desktop application for professional terrain analysis and environmental simulation.

![Build Status](https://img.shields.io/badge/build-passing-brightgreen)
![Version](https://img.shields.io/badge/version-1.0.0-blue)
![License](https://img.shields.io/badge/license-MIT-green)

## Overview

The Terrain Simulator is a full-fledged desktop application designed for researchers, engineers, and analysts to conduct sophisticated terrain analysis and run realistic environmental simulations.

## Quick Start

### Prerequisites
- C++20 compatible compiler (GCC 10+, Clang 11+, MSVC 2019+)
- CMake 3.20+
- GLFW3

### Build Instructions

1. **Install dependencies (macOS)**
   ```bash
   brew install glfw cmake
   ```

2. **Install dependencies (Ubuntu/Debian)**
   ```bash
   sudo apt-get install libglfw3-dev cmake build-essential
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

- **ESC**: Exit application
- **Close Window**: Exit application

## Features

### 🗺️ Terrain Engine (Planned)
- Multi-format support for DEM, GeoTIFF, and shapefile data
- Real-time 3D terrain rendering
- Line of sight calculations

### 🎯 Simulation Engine (Planned)
- Environmental scenario simulation
- Dynamic parameter processing
- Physics-based interactions

### 🤖 AI System (Planned)
- Reinforcement learning capabilities
- Multiple complexity levels
- Dynamic decision making

## License

This project is licensed under the MIT License.
