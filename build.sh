#!/bin/bash

# Build script for wya
set -e

echo "Building wya..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make

echo "Build complete! Executable is at build/bin/wya"
echo ""
echo "You can test it with:"
echo "  ./build/bin/wya help"
echo "  ./build/bin/wya scan -key basic --allow" 