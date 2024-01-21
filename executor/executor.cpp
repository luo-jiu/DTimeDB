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
        const std::shared_ptr<ExecutionPlanNode> & node) {
    if (node == nullptr) {
        return;  // 结束递归
    }

    // 特殊节点特殊处理 show
    if (node->name() == "show") {
        auto show_node = std::dynamic_pointer_cast<ShowNode>(node);
        if (show_node->m_type == "database") {
            node->show_database();
        }
        else  // table
        {
            // 遍历每个引擎，让他们打印出自己数据库对应的表
            for (auto &engine_it: m_engine_map) {
                node->execute(*engine_it.second);
            }
        }
    }
    else if (node->name() == "use")
    {
        for (auto &engine_it: m_engine_map) {
            node->execute(*engine_it.second);
        }
    }
    else
    {
        // 执行当前节点的操作(抉择是哪个引擎)
        auto m_tsm = m_engine_map["tsm"];
        node->execute(*m_tsm);
    }

    // 递归执行子节点
    execute_plan(node->get_child());
}

bool Executor::insert()
{
//    m_engine->insert.cpp();
}

//void Executor::load_database(
//        string & db_name)
//{
//    m_current_db = db_name;
//    std::cout << "use:" << db_name << std::endl;
//}
//
//string Executor::get_database()
//{
//    return m_current_db;
//}