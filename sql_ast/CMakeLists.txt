cmake_minimum_required(VERSION 3.23)
project(sql_ast)

set(CMAKE_CXX_STANDARD 11)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

include_directories(./)

file(GLOB_RECURSE SOURCES "token/*.cpp" "json/*.cpp" "lexer/*.cpp" "ast/*.cpp" "parser/*.cpp")

add_executable(token ${SOURCES} token.cpp)
add_executable(ast ${SOURCES} ast.cpp)