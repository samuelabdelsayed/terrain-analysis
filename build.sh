#!/bin/bash

echo "=== Building Enhanced Terrain Simulator ==="

mkdir -p build
cd build

if command -v clang++ &> /dev/null; then
    COMPILER="clang++"
    echo "Using Clang++: $(clang++ --version | head -n1)"
else
    echo "Error: No suitable C++ compiler found"
    exit 1
fi

echo "Building enhanced terrain simulator..."

if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "Building for macOS..."
    
    # Install dependencies
    if ! brew list glm &> /dev/null; then
        echo "Installing GLM..."
        brew install glm
    fi
    
    if ! brew list glfw &> /dev/null; then
        echo "Installing GLFW..."
        brew install glfw
    fi
    
    echo "Compiling terrain simulator..."
    
    $COMPILER -std=c++20 \
        -I../include \
        -I/opt/homebrew/include \
        -L/opt/homebrew/lib \
        -DGL_SILENCE_DEPRECATION \
        -Wno-deprecated-declarations \
        -O2 \
        ../src/main.cpp \
        ../src/core/Application.cpp \
        ../src/graphics/Camera.cpp \
        ../src/graphics/EntitySymbols.cpp \
        ../src/terrain/TerrainEngine.cpp \
        ../src/simulation/Unit.cpp \
        ../src/simulation/SimulationEngine.cpp \
        ../src/data/DatabaseManager.cpp \
        ../src/ai/AISystem.cpp \
        -lglfw \
        -framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL \
        -o TerrainSimulator
    
    if [ $? -eq 0 ]; then
        echo ""
        echo "🎉 Enhanced Terrain Simulator build successful!"
        echo ""
        echo "🚀 Features:"
        echo "   📊  Entity visualization (Rectangles=Allied, Diamonds=Opposition)"
        echo "   👥  Personnel: Crossed lines symbol"
        echo "   🚗  Vehicles: Oval with directional indicator"
        echo "   🎯  Equipment: Square with extension"
        echo "   👁️  Sensors: Circle with crosshairs"
        echo "   💡  Enhanced lighting and sky"
        echo ""
        echo "To run: cd build && ./TerrainSimulator"
        echo ""
        echo "You'll now see proper entity symbols instead of dots! 📊"
    else
        echo "❌ Build failed"
    fi
fi

echo ""
echo "=== Build Complete ==="
