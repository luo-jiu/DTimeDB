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
    }
    else
    {
        i->m_type = "table";
    }
    i->m_name = node->m_name;
    return i;
}