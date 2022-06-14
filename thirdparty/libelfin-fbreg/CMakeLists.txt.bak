cmake_minimum_required(VERSION 3.1)
project(libelfin)

add_library(libelfin)


file(GLOB_RECURSE LIBELFIN_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")

target_sources(libelfin PRIVATE ${LIBELFIN_SOURCES})

target_include_directories(libelfin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dwarf")
target_include_directories(libelfin PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/elf")


