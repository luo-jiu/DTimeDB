#include "evaluator/evaluator.h"
using namespace dt::evaluator;
using namespace dt::execution;

std::shared_ptr<ExecutionPlanNode> Evaluator::eval_integer(
        const std::shared_ptr<ast::Integer> & node)
{
    return new_integer(node->m_value);
}