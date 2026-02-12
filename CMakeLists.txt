cmake_minimum_required(VERSION 3.16)
project(flexiwin VERSION 0.1 LANGUAGES C CXX)

# Use modern C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


find_package(PkgConfig REQUIRED)
pkg_check_modules(WAYLAND REQUIRED wayland-client)


# Public include directory
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${WAYLAND_INCLUDE_DIRS}
)

# Library target
add_library(flexiwin STATIC
  src/wayland-callback.cpp
  src/xdg-output-protocol.c
  src/xdg-shell-protocol.c
  src/xdg/xdg.cpp
  src/flexiwin.cpp
)

target_include_directories(flexiwin PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(flexiwin ${WAYLAND_LIBRARIES} xkbcommon rt)


# Example executable
add_executable(hello_world examples/hello_world.cpp)
target_link_libraries(hello_world PRIVATE flexiwin)
