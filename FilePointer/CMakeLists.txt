cmake_minimum_required(VERSION 3.23)
project(FilePointer)

set(CMAKE_CXX_STANDARD 14)

add_executable(FilePointer main.cpp page_block.cpp)

#include_directories(page)
#add_subdirectory(page)

#include_directories(page_offset)
