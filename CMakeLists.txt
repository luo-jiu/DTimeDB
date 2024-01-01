cmake_minimum_required(VERSION 3.23)
project(DTimeDB)

set(CMAKE_CXX_STANDARD 17)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

include_directories(./)

file(GLOB_RECURSE SOURCES "token/*.cpp" "lexer/*.cpp" "json/*.cpp" "ast/*.cpp"
        "parser/*.cpp" "engine/tsm/*.cpp" "engine/circularList/*.cpp")

add_executable(token ${SOURCES} token.cpp)
add_executable(ast ${SOURCES} ast.cpp
        engine/circularList/include/data_def.h
        engine/circularList/include/circular_list.h
        pool/threadPool.cpp
        pool/threadPool.h
        engine/tsm/file_manager.cpp
        engine/circularList/include/circular_list_struct.h
        engine/circularList/include/circular_tools.h
        engine/circularList/test/CircularListTest.cpp
        engine/circularList/include/circular_list_header.h
        engine/circularList/include/storage/page/page.h
        engine/circularList/include/storage/table/tuple.h
        engine/circularList/include/type/value.h
        engine/circularList/storage/disk/disk_manager.cpp
        engine/circularList/include/storage/disk/disk_manager.h
        engine/circularList/include/buffer/lru_replacer.h
        engine/circularList/include/storage/table/table.h
        engine/circularList/include/storage/table/table_iterator.h
        engine/circularList/include/storage/table/table_heap.h
        engine/circularList/include/common/rwlatch.h
        engine/circularList/include/catalog/column.h
        engine/circularList/include/catalog/schema.h
        engine/circularList/include/catalog/catalog.h
        engine/circularList/include/storage/page/circular_list_page.h
        engine/circularList/include/common/config.h
        engine/circularList/storage/table/table.cpp
        engine/circularList/recovery/log_manager.h
        engine/circularList/include/type/type_id.h
        engine/circularList/include/common/get_timestamp.h
        engine/circularList/catelog/schema.cpp
        engine/circularList/include/catalog/table_generator.h
        engine/circularList/include/storage/page/header_page.h
        engine/circularList/storage/table/tuple.cpp
        engine/circularList/type/value.cpp
        engine/circularList/type/type.cpp
        engine/circularList/include/type/type.h
        engine/circularList/include/type/integer.h
        engine/circularList/include/type/varchar.h
        engine/circularList/include/type/boolean.h
        engine/circularList/include/type/timestamp.h
        engine/circularList/include/type/decimal.h
        exception/exception.h
)

#add_executable(ast ${SOURCES} ast.cpp)

add_executable(tsm ${SOURCES} tsm.cpp)
add_executable(test ${SOURCES} test.cpp)


