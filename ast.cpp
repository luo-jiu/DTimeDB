#include <iostream>

#include <lexer/lexer.h>
using namespace dt::lexer;

#include <parser/parser.h>
using namespace dt::parser;

#include <evaluator/evaluator.h>
using namespace dt::evaluator;

#include <executor/executor.h>
using namespace dt::executor;

/**
 * ast抽象语法树 test
 * @return
 */
int main()
{
    std::shared_ptr<Lexer> lexer(new Lexer("./../sql.txt"));
    std::shared_ptr<Parser> parser(new Parser(lexer));
    std::shared_ptr<Environment> env(new Environment());
    std::shared_ptr<Evaluator> evaluator(new Evaluator());
    std::shared_ptr<RootNode> root = std::make_shared<RootNode>();
    std::shared_ptr<Executor> executor(new Executor());

    // 构建ast
    auto program = parser->parse_program();

    // 递归构建执行计划
    evaluator->eval(program, env.get(), root);

    // 递归运行执行计划
//    executor->execute_plan(root);

    Json json = program->json();

    std::ofstream ofs("./../sql.json");
    ofs << json.str();
    ofs.close();
    return 0;
}
