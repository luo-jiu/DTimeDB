#include <ast/header.h>
using namespace dt::ast;

#include <lexer/lexer.h>
using namespace dt::lexer;

#include <parser/parser.h>
using namespace dt::parser;

#include "executor/operator/query_plan.h"
using namespace dt::evaluator;

#include <engine/file_manager/file_path_manager.h>
using namespace dt::tsm;

#include <iostream>
using std::string;

int main() {
    string table_name = "school";
    string temp = "tsm";

    FilePathManager file_path_manager("./../dbs");

    for(int i = 10; i > 0; i--)
    {
        string filePath = file_path_manager.create_file(table_name, temp);
        std::cout << "File created at: " << filePath << std::endl;
    }


    // 假设其他操作...
//
//    if (file_path_manager.delete_file(filePath)) {
//        std::cout << "File deleted successfully." << std::endl;
//    }

    return 0;




//    // 准备数据
//    std::vector<Row> table_data = {{1, "Alice", 20},{2, "Bob", 30},{3, "Carol", 25}};
//
//    // 构建查询计划
//    ScanOperator scan(table_data);
//    SelectOperator select(&scan, [](const Row & row){
//        return row.m_age > 20;
//    });
//    ProjectOperator project(&select, [](const Row & row){
//        std::cout << "ID: " << row.m_id << ", Name: " << row.m_name << std::endl;
//    });
//
//    // 执行查询计划
//    Row row;
//    while (project.get_next(row))
//    {
//    }
//
//    return 0;
}
