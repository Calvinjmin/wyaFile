#!/bin/bash

# Local installation script for wya
set -e

echo "Installing wya..."

# Build the project
./build.sh

# Install to system
sudo cmake --install build

echo "Installation complete!"
echo ""
echo "You can now use 'wya' from anywhere in your terminal:"
echo "  wya help"
echo "  wya scan -key basic --allow"
echo "  wya scan -dir /path/to/directory --allow"
echo ""
echo "To uninstall, run: sudo rm /usr/local/bin/wya" 