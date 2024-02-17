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

const std::string prompt = "<<";

int main()
{
    std::shared_ptr<dt::execution::RootNode> root;
    std::shared_ptr<Executor> executor(new Executor());

    std::cout << "Welcome to DTimeDB!" << std::endl;
    std::shared_ptr<Environment> env(new Environment());

    // 递归向下求值
    std::shared_ptr<Evaluator> evaluator(new Evaluator());
    while (true)
    {
        root = std::make_shared<dt::execution::RootNode>();

        std::cout << prompt;
        std::string text;
        std::getline(std::cin, text);

        if (text == "exit")  // 退出出口
        {
            std::cout << "Bye!" << std::endl;
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
        auto evaluated = evaluator->eval(program, env.get(), root);
        executor->execute_plan(root);
    }
}