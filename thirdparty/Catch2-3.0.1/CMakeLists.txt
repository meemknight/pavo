cmake_minimum_required(VERSION 3.1)
project(catch2)

add_library(catch2)
target_sources(catch2 PRIVATE "catch_amalgamated.cpp")
target_include_directories(catch2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/")
#target_link_libraries(catch2 PUBLIC glm glad stb_image stb_truetype)