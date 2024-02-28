cmake_minimum_required(VERSION 3.23)
project(Proto)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory(./ SRC)

add_library(proto ${SRC})
target_link_libraries(proto protobuf)