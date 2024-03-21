cmake_minimum_required(VERSION 3.23)
project(DTimeDB)

set(CMAKE_CXX_STANDARD 17)
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
include_directories(./)

# 添加子目录
add_subdirectory(externals/snappy)
# 添加测试用目录
add_subdirectory(proto)

file(GLOB_RECURSE SOURCES "token/*.cpp" "lexer/*.cpp" "json/*.cpp" "ast/*.cpp"
        "parser/*.cpp" "engine/tsm/*.cpp" "engine/circularList/*.cpp" "executor/*.cpp"
        "file_manager/*.cpp" "execution_plan/*.cpp" "evaluator/*.cpp" "thread_manager/*.cpp"
        "wal/*.cpp" "proto/*.cpp" "server/src/*.cpp" )

# 链接静态库
link_libraries(proto)

add_executable(ast ${SOURCES} ast.cpp)
add_executable(eval_repl ${SOURCES} eval_repl.cpp)
add_executable(eval_repl_pro ${SOURCES} eval_repl_pro.cpp)
add_executable(token ${SOURCES} token.cpp)
add_executable(tsm ${SOURCES} tsm.cpp)
add_executable(client ${SOURCES} client.cpp)
add_executable(server ${SOURCES} server.cpp)
add_executable(test_snappy ${SOURCES} test_snappy.cpp)
add_executable(test_tool ${SOURCES} test_tool.cpp)

# 链接 Snappy 库到可执行文件
target_link_libraries(ast snappy)
target_link_libraries(eval_repl snappy)
target_link_libraries(eval_repl_pro snappy)
target_link_libraries(token snappy)
target_link_libraries(tsm snappy)
target_link_libraries(test_snappy snappy)
target_link_libraries(test_tool snappy)
target_link_libraries(server pthread)
target_link_libraries(client pthread)


