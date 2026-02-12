cmake_minimum_required(VERSION 3.16)
project(flexiwin VERSION 0.1 LANGUAGES CXX)

# Use modern C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Public include directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Library target
add_library(flexiwin
  src/wayland.cpp
  src/xdg/xdg.cpp
  src/wayland-callback.cpp
  src/xdg-output.c
  src/xdg-shell-protocol.c
)

target_include_directories(flexiwin PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)


# Example executable
add_executable(hello_world examples/hello_world.cpp)
target_link_libraries(hello_world PRIVATE flexiwin wayland-client xkbcommon)
