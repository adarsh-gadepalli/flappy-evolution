cmake_minimum_required(VERSION 3.10)
project(FlappyBird)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find SFML
find_package(SFML 3.0.2 COMPONENTS Graphics Window System REQUIRED)

# Add executable
add_executable(flappy main.cpp)

# Link SFML libraries (SFML 3.0 uses SFML:: namespace targets)
target_link_libraries(flappy SFML::Graphics SFML::Window SFML::System)

