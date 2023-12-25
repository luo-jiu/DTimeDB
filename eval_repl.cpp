#include <iostream>
#include <string>

#include <lexer/lexer.h>
using namespace dt::lexer;

#include <parser/parser.h>
using namespace dt::parser;

#include <evaluator/evaluator.h>
using namespace dt::evaluator;

const string prompt = " <<";

int main()
{
    std::cout << "Welcome to DTimeDB!" << std::endl;
    std::shared_ptr<Environment> env(new Environment());

    // 递归向下求值
    std::shared_ptr<Evaluator> evaluator(new Evaluator());
    while (true)
    {
        std::cout << prompt;
        string text;
        std::getline(std::cin, text);

        if (text == "exit")  // 退出出口
        {
            return 0;
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
        std::shared_ptr<ExecutionPlanNode> root;
        auto evaluated = evaluator->eval(program, env.get(), root);
        if (evaluated)
        {
            if (evaluated->type() != ExecutionPlanNode::OBJECT_NULL)  // 不为空才打印
            {
                std::cout << evaluated->str() << std::endl;
            }
        }
    }
}