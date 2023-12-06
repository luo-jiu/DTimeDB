cmake_minimum_required(VERSION 3.23)
project(DTimeDB)

set(CMAKE_CXX_STANDARD 14)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

include_directories(./)

file(GLOB_RECURSE SOURCES "token/*.cpp" "lexer/*.cpp" "json/*.cpp" "ast/*.cpp"
        "parser/*.cpp" "engine/tsm/*.cpp" "engine/circularList/*.cpp")

add_executable(token ${SOURCES} token.cpp)
add_executable(ast ${SOURCES} ast.cpp
        engine/circularList/controll/circularList.cpp
        engine/circularList/controll/page.cpp
        engine/circularList/include/data_def.h
        engine/circularList/include/circular_list.h
        pool/threadPool.cpp
        pool/threadPool.h
        engine/tsm/file_manager.cpp
        engine/circularList/include/circular_list_struct.h
        engine/circularList/file/file.h
        engine/circularList/include/circular_tools.h
        engine/circularList/test/CircularListTest.cpp
        engine/circularList/include/circular_list_header.h
)

#add_executable(ast ${SOURCES} ast.cpp)

add_executable(tsm ${SOURCES} tsm.cpp)
add_executable(test ${SOURCES} test.cpp)


