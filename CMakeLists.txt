cmake_minimum_required(VERSION 3.10)
project(calpath_wasm)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/orbitTools/core
    ${CMAKE_SOURCE_DIR}/src/orbitTools/orbit
)

# Add source files
set(SOURCES
    # Main source files
    src/calculator.cpp
    src/Color.cpp
    src/CRegion.cpp
    src/DateTime.cpp
    src/JulianConvert.cpp
    src/Logger.cpp
    src/MyPoint.cpp
    src/RelationOperator.cpp
    src/Satellite.cpp
    src/Sensor.cpp
    src/StripUnit.cpp
    src/TargetArea.cpp
    src/TrackPoint.cpp
    # OrbitTools Core
    src/orbitTools/core/cEci.cpp
    src/orbitTools/core/cJulian.cpp
    src/orbitTools/core/cSite.cpp
    src/orbitTools/core/cTLE.cpp
    src/orbitTools/core/cVector.cpp
    src/orbitTools/core/coord.cpp
    src/orbitTools/core/globals.cpp
    # OrbitTools Orbit
    src/orbitTools/orbit/cNoradBase.cpp
    src/orbitTools/orbit/cNoradSDP4.cpp
    src/orbitTools/orbit/cNoradSGP4.cpp
    src/orbitTools/orbit/cOrbit.cpp
    src/orbitTools/orbit/cSatellite.cpp
)

# Add header files
set(HEADERS
    # Main headers
    src/Color.h
    src/CRegion.h
    src/DateTime.h
    src/JulianConvert.h
    src/Logger.h
    src/MyPoint.h
    src/RelationOperator.h
    src/Satellite.h
    src/Sensor.h
    src/StripUnit.h
    src/TargetArea.h
    src/TrackPoint.h
    # OrbitTools Core headers
    src/orbitTools/core/cEci.h
    src/orbitTools/core/cJulian.h
    src/orbitTools/core/cSite.h
    src/orbitTools/core/cTLE.h
    src/orbitTools/core/cVector.h
    src/orbitTools/core/coord.h
    src/orbitTools/core/coreLib.h
    src/orbitTools/core/exceptions.h
    src/orbitTools/core/globals.h
    # OrbitTools Orbit headers
    src/orbitTools/orbit/cNoradBase.h
    src/orbitTools/orbit/cNoradSDP4.h
    src/orbitTools/orbit/cNoradSGP4.h
    src/orbitTools/orbit/cOrbit.h
    src/orbitTools/orbit/cSatellite.h
    src/orbitTools/orbit/orbitLib.h
)

# Create the executable
add_executable(satpath ${SOURCES} ${HEADERS})

# Set output name and directory
set(CMAKE_EXECUTABLE_SUFFIX ".js")

# Configure Emscripten flags
set(EMSCRIPTEN_LINK_FLAGS "-s WASM=1 -s EXPORT_NAME='createModule' -s MODULARIZE=1 -s ENVIRONMENT='web,worker' -s NO_EXIT_RUNTIME=1 -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap'] -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=1 --bind -O0")
string(REPLACE ";" " " EMSCRIPTEN_LINK_FLAGS_STR "${EMSCRIPTEN_LINK_FLAGS}")
set_target_properties(satpath PROPERTIES LINK_FLAGS "${EMSCRIPTEN_LINK_FLAGS_STR}")
