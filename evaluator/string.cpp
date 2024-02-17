#include "evaluator/evaluator.h"
using namespace dt::evaluator;
using namespace dt::execution;

std::shared_ptr<ExecutionPlanNode> Evaluator::eval_string(
        const std::shared_ptr<ast::String> & node)
{
    return new_string(node->m_value);
}