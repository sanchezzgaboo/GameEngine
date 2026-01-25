cmake_minimum_required(VERSION 3.16)
project(GameEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "Configuring GameEngine project...")

# Set the path to SFML if it's in lib/SFML-3.0.2
set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/lib/SFML-3.0.2" ${CMAKE_PREFIX_PATH})

# Find SFML
find_package(SFML 3.0.2 REQUIRED COMPONENTS Graphics Window System Audio)
if(NOT SFML_FOUND)
    message(FATAL_ERROR "SFML 3.0.2 not found. Please ensure SFML is installed. If using the provided SFML in lib/SFML-3.0.2, check that it's built and CMAKE_PREFIX_PATH is correct. Download from https://www.sfml-dev.org/download.php")
endif()
message(STATUS "SFML found: ${SFML_VERSION} at ${SFML_DIR}")
message(STATUS "SFML_INCLUDE_DIRS: ${SFML_INCLUDE_DIRS}")

# Add SFML lib directory
link_directories(${CMAKE_SOURCE_DIR}/lib/SFML-3.0.2/lib)

# Add the imgui library (merged with imgui-sfml)
message(STATUS "Adding ImGui-SFML library...")
add_library(imgui-sfml STATIC
    lib/imgui/imgui.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_widgets.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/imgui-SFML.cpp
)

target_include_directories(imgui-sfml PUBLIC 
    lib/imgui
    ${CMAKE_SOURCE_DIR}/lib/SFML-3.0.2/include
)

target_link_libraries(imgui-sfml PUBLIC 
    sfml-graphics
    sfml-window
    sfml-system
    opengl32
)

target_compile_definitions(imgui-sfml PUBLIC IMGUI_SFML)

message(STATUS "ImGui-SFML library configured.")



# Add the executable
message(STATUS "Creating GameEngine executable...")
add_executable(GameEngine 
    src/main.cpp
    src/scene/Camera.cpp
)

message(STATUS "Linking Headers...")
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/math
    ${CMAKE_CURRENT_SOURCE_DIR}/src/geometry
    ${CMAKE_CURRENT_SOURCE_DIR}/src/render
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scene
)

# Link libraries
target_link_libraries(GameEngine PRIVATE 
    imgui-sfml
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

message(STATUS "GameEngine project configured successfully. Run 'cmake --build build' to build.")
