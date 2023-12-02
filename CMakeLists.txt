cmake_minimum_required(VERSION 3.23)
project(DTimeDB)

set(CMAKE_CXX_STANDARD 11)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

include_directories(./)

file(GLOB_RECURSE SOURCES "token/*.cpp" "lexer/*.cpp" "json/*.cpp" "ast/*.cpp"
        "parser/*.cpp" "engine/tsm/*.cpp" "")

add_executable(token ${SOURCES} token.cpp)
add_executable(ast ${SOURCES} ast.cpp
        engine/circularList/page/circularLIst.cpp
        engine/circularList/page/page.cpp
        engine/circularList/include/dataDef.h
        engine/circularList/include/circularList.h
        pool/threadPool.cpp
        pool/threadPool.h
)
add_executable(tsm ${SOURCES} tsm.cpp)
add_executable(test ${SOURCES} test.cpp)


