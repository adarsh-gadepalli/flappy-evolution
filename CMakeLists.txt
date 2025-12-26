cmake_minimum_required(VERSION 3.10)
project(FlappyBird)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find SFML
find_package(SFML 3.0.2 COMPONENTS Graphics Window System REQUIRED)

# Add main game executable (with SFML)
add_executable(flappy main.cpp renderer.cpp simulation.cpp neural_network.cpp)
target_link_libraries(flappy SFML::Graphics SFML::Window SFML::System)

# Add training executable (no SFML needed)
add_executable(train train.cpp evolution.cpp neural_network.cpp simulation.cpp)
target_include_directories(train PRIVATE ${CMAKE_SOURCE_DIR})

# Copy compile_commands.json to root directory for IDE (after configuration)
# Note: This may fail in some environments, but won't prevent the build
add_custom_command(TARGET flappy POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_SOURCE_DIR}/compile_commands.json
    COMMENT "Copying compile_commands.json to project root"
    VERBATIM
)

