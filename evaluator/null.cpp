#include "evaluator/evaluator.h"
using namespace dt::evaluator;
using namespace dt::execution;

std::shared_ptr<ExecutionPlanNode> Evaluator::eval_null(
        const std::shared_ptr<ast::Null> & node)
{
    return new_null();
}