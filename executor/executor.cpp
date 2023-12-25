#include <executor/executor.h>
using namespace dt::executor;

/**
 * 生成AST 抽象语法树
 */
std::shared_ptr<Program> Executor::generate_ast_tree(
        string & sql)
{
    std::shared_ptr<Lexer> lexer(new Lexer());
    lexer->set_input(sql);

    std::shared_ptr<Parser> parser(new Parser(lexer));

    auto program = parser->parse_program();
    return program;
}

/**
 * 生成执行计划
 */
void Executor::generate_execute_plan(string & sql)
{
    std::shared_ptr<Environment> env(new Environment());
    std::shared_ptr<Evaluator> evaluator(new Evaluator());

    // 递归向下求值
//    auto evaluated = evaluator->eval(generate_ast_tree(sql), env.get());


}

/**
 * 递归执行 执行计划
 * @param node
 */
void Executor::execute_plan(
        const std::shared_ptr<ExecutionPlanNode> & node)
{
    if (node == nullptr)
    {
        return;  // 结束递归
    }

    // 执行当前节点的操作
    node->execute(m_engine);

    // 递归执行子节点
    execute_plan(node->get_child());
}

bool Executor::insert()
{
//    m_engine->insert.cpp();
}