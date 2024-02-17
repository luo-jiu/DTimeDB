#include "evaluator/evaluator.h"
using namespace dt::evaluator;
using namespace dt::execution;

std::shared_ptr<ExecutionPlanNode> Evaluator::eval_use(
        const std::shared_ptr<ast::Use> & node)
{
    std::shared_ptr<UseNode> i(new UseNode());
    i->m_database = node->m_database_name;
    return i;
}