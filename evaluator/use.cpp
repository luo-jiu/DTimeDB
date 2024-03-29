#include "evaluator/evaluator.h"
using namespace dt::evaluator;
using namespace dt::execution;

std::shared_ptr<ExecutionPlanNode> Evaluator::eval_use(
        const std::shared_ptr<ast::Use> & node)
{
    std::shared_ptr<UseNode> i(new UseNode());
    i->m_database_name = node->m_database_name;
    // 给执行计划生成树设置当前数据库
    if (use_database(node->m_database_name))
    {
        // 成功切换了数据库才会去加载对应数据库的元数据
        i->m_tb_engine = m_tab_eng_manager.load_database_metadata(node->m_database_name);
    }
    else
    {
        // 说明数据库不存在
        std::cout << "The '"<< node->m_database_name <<"' Databases does not exist.\n";
        return nullptr;
    }
    return i;
}