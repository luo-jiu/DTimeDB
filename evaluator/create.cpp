#include "evaluator/evaluator.h"
using namespace dt::evaluator;
using namespace dt::execution;
using namespace dt::ast;

std::shared_ptr<ExecutionPlanNode> Evaluator::eval_create(
        const std::shared_ptr<ast::Create> & node)
{
    std::shared_ptr<CreateNode> i(new CreateNode());
    if (node->m_type == Create::Type::CREATE_DATABASE)
    {
        i->m_type = "database";
        if (!node->m_name.empty())
        {
            m_tab_eng_manager.create_database(node->m_name);
        }
        else
        {
            std::cout << "the database name is empty\n";
        }
    }
    else
    {
        i->m_type = "table";
        m_tab_eng_manager.create_table(m_curr_db_name, node->m_name, node->m_engine);
    }
    i->m_name = node->m_name;

    return i;
}