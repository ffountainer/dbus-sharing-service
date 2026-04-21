#!/usr/bin/env bash

set -e 

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"

echo "Will clear build diretory in case if it exists"
rm -rf "$BUILD_DIR"

echo "Creating new build directory!"
mkdir -p "$BUILD_DIR"

echo "Running CMake"
cd "$BUILD_DIR"
cmake ..

echo "Compiling project"
cmake --build .