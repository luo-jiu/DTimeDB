#include <lexer/lexer.h>
using namespace dt::lexer;

#include <parser/parser.h>
using namespace dt::parser;

#include <evaluator/evaluator.h>
using namespace dt::evaluator;

#include <executor/executor.h>
using namespace dt::executor;

#include <iostream>
#include <string>
#include <random>

const string prompt = "<<";

#define TEST_COUNT 99

// 随机选择一个字符串数组中的元素
std::string get_random_element(const std::vector<std::string>& strArray) {
    if (strArray.empty()) {
        return ""; // 如果数组为空，返回空字符串
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 864000);  // 定义时间范围（比如，240小时 = 864000秒）


    // 返回随机选择的元素
    return strArray[dis(gen) % strArray.size()];
}

int main()
{
    std::shared_ptr<RootNode> root;
    std::shared_ptr<Executor> executor(new Executor());

    std::cout << "Welcome to DTimeDB!" << std::endl;
    std::shared_ptr<Environment> env(new Environment());

    // 递归向下求值
    std::shared_ptr<Evaluator> evaluator(new Evaluator());

    // 先模拟输入10次insert 插入数据以便于调试
    size_t count = TEST_COUNT;
    std::vector<string> sql = {
            "insert into school,local='中德',class='软工1班' '温度'=23.9",
            "insert into school,local='中德',class='软工1班' '温度'=22.8",
            "insert into school,local='中德',class='软工1班' '温度'=21.7",
            "insert into school,local='中德',class='软工1班' '温度'=20.6"};
    bool run = true;
    string text;
    while (true)
    {
        root = std::make_shared<RootNode>();
        if (run)
        {
            text = "use db_test";
            run = false;
        }

        // 词法分析器
        std::shared_ptr<Lexer> lexer(new Lexer(text.c_str(), text.size()));

        // 语法分析器
        std::shared_ptr<Parser> parser(new Parser(lexer));

        // 构建抽象语法树
        auto program = parser->parse_program();
        auto errors = parser->errors();
        if (!errors.empty())
        {
            for (auto & error : errors)
            {
                std::cout << error << std::endl;
            }
            continue;
        }

        // 递归向下求值
        auto evaluated = evaluator->eval(program, env.get(), root);
        executor->execute_plan(root);

        if (count)  // 自动输入测试程序
        {
            text = get_random_element(sql);
            count--;
        }
        else  // 手动输入程序
        {
            std::cout << prompt;
            std::getline(std::cin, text);

            if (text == "exit")  // 退出出口
            {
                std::cout << "Bye!" << std::endl;
                return 0;
            }
        }
    }
}