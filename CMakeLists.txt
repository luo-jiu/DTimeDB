cmake_minimum_required(VERSION 3.23)
project(DTimeDB)

set(CMAKE_CXX_STANDARD 17)
set(BOOST_ROOT "/usr/local/include/boost")
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
include_directories(/usr/local/include)
link_directories(/usr/local/lib)
include_directories(./)

file(GLOB_RECURSE SOURCES "token/*.cpp" "lexer/*.cpp" "json/*.cpp" "ast/*.cpp"
        "parser/*.cpp" "engine/tsm/*.cpp"  "executor/*.cpp"
        "file_manager/*.cpp" "execution_plan/*.cpp" "evaluator/*.cpp" "thread_manager/*.cpp" "server/src/*.cpp")
add_executable(client ${SOURCES} client.cpp)
add_executable(ast ${SOURCES} ast.cpp
        server/include/my_database.h)
add_executable(eval_repl ${SOURCES} eval_repl.cpp)
add_executable(token ${SOURCES} token.cpp)
add_executable(tsm ${SOURCES} tsm.cpp)
add_executable(test ${SOURCES} test.cpp)
add_executable(server ${SOURCES} server.cpp)


