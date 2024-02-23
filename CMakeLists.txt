cmake_minimum_required(VERSION 3.23)
project(DTimeDB)

set(CMAKE_CXX_STANDARD 17)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

include_directories(./)

add_subdirectory(externals/snappy)

file(GLOB_RECURSE SOURCES "token/*.cpp" "lexer/*.cpp" "json/*.cpp" "ast/*.cpp"
        "parser/*.cpp" "engine/tsm/*.cpp" "engine/circularList/*.cpp" "executor/*.cpp"
        "file_manager/*.cpp" "execution_plan/*.cpp" "evaluator/*.cpp" "thread_manager/*.cpp"
        "wal/*.cpp" "")

add_executable(ast ${SOURCES} ast.cpp)
add_executable(eval_repl ${SOURCES} eval_repl.cpp)
add_executable(eval_repl_pro ${SOURCES} eval_repl_pro.cpp)
add_executable(token ${SOURCES} token.cpp)
add_executable(tsm ${SOURCES} tsm.cpp)
add_executable(test ${SOURCES} test.cpp)
add_executable(test_snappy ${SOURCES} test_snappy.cpp)

# 链接 Snappy 库到可执行文件
target_link_libraries(ast snappy)
target_link_libraries(eval_repl snappy)
target_link_libraries(eval_repl_pro snappy)
target_link_libraries(token snappy)
target_link_libraries(tsm snappy)
target_link_libraries(test snappy)
target_link_libraries(test_snappy snappy)


