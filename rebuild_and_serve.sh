#!/bin/zsh

# Set up Emscripten environment
echo "===== Set up Emscripten environment ====="
source ./emsdk/emsdk_env.sh

# Clean previous build
echo "===== Clean previous build ====="
rm -rf build/CMakeFiles
rm -f build/satpath.js build/satpath.wasm

# Configure project using emcmake
echo "===== Configure project using emcmake ====="
emcmake cmake -DCMAKE_BUILD_TYPE=Release .

# Compile WebAssembly code
echo "===== Compile WebAssembly code using emmake ====="
emmake make

# Copy build files to build directory
echo "===== Copy build files to build directory ====="
cp satpath.js satpath.wasm build/

# Kill all running Python HTTP servers
echo "===== Kill all running Python HTTP servers ====="
pkill -f "python.*http.server" || echo "No running HTTP server found"

# Start HTTP server
echo "===== Start HTTP server ====="
echo "Access at: http://localhost:8082"
python3 -m http.server 8082
