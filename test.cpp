#include <ast/header.h>
using namespace dt::ast;

#include <lexer/lexer.h>
using namespace dt::lexer;

#include <parser/parser.h>
using namespace dt::parser;

#include "executor/operator/query_plan.h"
using namespace dt::evaluator;

#include <file_manager/file_path_manager.h>
using namespace dt::file;

#include <executor/executor.h>
using namespace dt::executor;
#include <random>

#include <iostream>
using std::string;


// 随机选择一个字符串数组中的元素
std::string getRandomElement(const std::vector<std::string>& strArray) {
    if (strArray.empty()) {
        return ""; // 如果数组为空，返回空字符串
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 864000);  // 定义时间范围（比如，240小时 = 864000秒）


    // 返回随机选择的元素
    return strArray[dis(gen)%strArray.size()];
}

//#include <iostream>
#include <vector>
#include "engine/circularList/include/catalog/column.h"
#include "engine/circularList/include/catalog/schema.h"
#include "engine/circularList/include/catalog/table_generator.h"
//
//#include <engine/tsm/skip_list.h>
//using namespace dt::tsm;
//
using namespace ctl;
int main() {
    std::shared_ptr<Environment> env(new Environment());
    std::shared_ptr<Evaluator> evaluator(new Evaluator());
    std::shared_ptr<RootNode> root = std::make_shared<RootNode>();
    std::shared_ptr<Executor> executor(new Executor());
    std::vector<string> sqls = {"show databases", "show tables", "use db_test", "use xxx"};

    // 暴力测试
    for (int i = 0; i < 100000; ++i)
    {
        string randomElement = getRandomElement(sqls);
//        std::cout << "Random Element #" << i + 1 << ": " << randomElement << std::endl;

        std::shared_ptr<Lexer> lexer(new Lexer(randomElement.c_str(), randomElement.size()));
        std::shared_ptr<Parser> parser(new Parser(lexer));

        auto program = parser->parse_program();

        // 递归构建执行计划
        evaluator->eval(program, env.get(), root);

        // 递归运行执行计划
        executor->execute_plan(root);

        root = std::make_shared<RootNode>();

    }

    ctl::Column col1("name",ctl::VARCHAR,10);
    ctl::Column col2("age",ctl::INTEGER,5);
    std::vector<Column> columns={col1,col2};
    Schema schema(columns);
//    TableGenerator tableGenerator("mytable",schema);
    std::cout<<schema.ToString()<<std::endl;
    const std::vector<Column> &schemacol=schema.GetColumns();
    for(const auto& col:schemacol){
        std::cout<<"col name: "<<col.GetName()<<", Type: "<<col.GetType()<<std::endl;
    }
    ctl::Field intfield(INTEGER,5);
    std::cout<<intfield<<std::endl;
    ctl::Field varcharField(VARCHAR,"hello");
    std::cout<<varcharField<<std::endl;
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
