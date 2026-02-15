cmake_minimum_required(VERSION 3.16)
project(flexiwin VERSION 0.1 LANGUAGES C CXX)

# Use modern C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_BUILD_TYPE Debug)
set(OpenGL_GL_PREFERENCE GLVND)

find_package(PkgConfig REQUIRED)

pkg_check_modules(WAYLAND REQUIRED wayland-client wayland-egl)
pkg_check_modules(EGL REQUIRED egl)
pkg_check_modules(OPENGL REQUIRED opengl)


# Public include directory
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${WAYLAND_INCLUDE_DIRS}
  ${EGL_INCLUDE_DIRS}

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
target_link_libraries(flexiwin ${WAYLAND_LIBRARIES} ${EGL_LIBRARIES} xkbcommon rt)


# Example executable
add_executable(hello_world examples/hello_world.cpp)
add_executable(opengl_example examples/opengl_example.cpp)

target_link_libraries(hello_world PRIVATE flexiwin)
target_link_libraries(opengl_example PRIVATE flexiwin ${OPENGL_LIBRARIES})
