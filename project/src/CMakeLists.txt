# CMake configuration for the Group Project sources
#
# CMSC 23700 -- Introduction to Computer Graphics
# Autumn 2023
# University of Chicago
#
# COPYRIGHT (c) 2023 John Reppy
# All rights reserved.
#

# the executable is named "terra-view"
set(TARGET terra-view)

set(SRCS
  app.cpp
  camera.cpp
  frustum.cpp
  main.cpp
  map-cell.cpp
  map.cpp
  texture-cache.cpp
  vao.cpp
  window.cpp)

# path to CS237 Library include files
include_directories(${CS237_INCLUDE_DIR})

add_executable(${TARGET} ${SRCS})

target_link_libraries(${TARGET} cs237)
add_dependencies(${TARGET} part1-shaders)
