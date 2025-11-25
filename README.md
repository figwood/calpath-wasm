## Project Structure

- `src/`: C++ source code
  - `calculator.cpp`: Main calculation logic and Emscripten bindings
  - `Satellite.h/cpp`: Satellite orbit calculation
  - `Sensor.h/cpp`: Sensor model
  - `CRegion.h/cpp`: Region calculation
  - `orbitTools/`: Orbit calculation library
- `index.html`: Web frontend interface
- `CMakeLists.txt`: Build configuration file

## Tech Stack

- C++14
- Emscripten
- WebAssembly
- HTML5/CSS3/JavaScript

## Environment Setup

1. Install prerequisites: a recent `cmake`, `python3`, and build tools (`clang`/`llvm`) available on your system.
2. Install the pinned Emscripten toolchain: `./emsdk/emsdk install 3.1.62`.
3. Activate the toolchain: `./emsdk/emsdk activate 3.1.62`.
4. Load the environment in each new shell: `source ./emsdk/emsdk_env.sh`.

## Building

1. `source ./emsdk/emsdk_env.sh` to load the pinned Emscripten toolchain.
2. `emcmake cmake -S . -B build -DCMAKE_BUILD_TYPE=Release` to configure the project.
3. `cmake --build build --parallel` to compile the WebAssembly bundle (`satpath.js`, `satpath.wasm`).

You can also run `./rebuild_and_serve.sh` to perform the full clean build and launch a local static server on port 8082.

## Testing

Automated tests are not yet implemented. Run `./rebuild_and_serve.sh`, open `http://localhost:8082`, and validate functionality via the browser interface.
