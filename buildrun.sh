#!/bin/bash

mkdir -p build
cd build
cmake ..
echo "--- Compiling ---"
cmake --build . --target SandboxApp

if [ $? -eq 0 ]; then
    echo "YIPPEE"
    cd ..
    if [ -f "./build/sandbox/SandboxApp" ]; then
        ./build/sandbox/SandboxApp
    else
        ./build/SandboxApp
    fi
else
    echo "OOPSIESSS"
    exit 1
fi