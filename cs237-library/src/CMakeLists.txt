# CMake configuration for CS237 utility library
#
# CMSC 23700 -- Introduction to Computer Graphics
# Autumn 2023
# University of Chicago
#
# COPYRIGHT (c) 2023 John Reppy
# All rights reserved.
#

set(SRCS
  aabb.cpp
  application.cpp
  depth-buffer.cpp
  image.cpp
  json.cpp
  json-parser.cpp
  memory-obj.cpp
  mtl-reader.cpp
  obj-reader.cpp
  obj.cpp
  shader.cpp
  texture.cpp
  tqt.cpp
  window.cpp)

add_library(cs237
  STATIC
  ${SRCS})
