
cmake_minimum_required(VERSION 3.20)

project(ForgeEngine
    VERSION 0.1.0
    DESCRIPTION "Open-source 2D and 3D Game Engine"
    LANGUAGES CXX
)

# C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(FORGE_BUILD_EDITOR "Build Forge Engine Editor" ON)
option(FORGE_BUILD_TESTS "Build Forge Engine Tests" ON)

# Engine source directory
add_subdirectory(Engine)

# Sandbox application
add_subdirectory(Sandbox)

# Tests
if(FORGE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(Tests)
endif()
