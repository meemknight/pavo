cmake_minimum_required(VERSION 3.1)
project(lineNoise)

add_library(lineNoise)



target_sources(lineNoise PRIVATE "linenoise.c")

target_include_directories(lineNoise PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")


