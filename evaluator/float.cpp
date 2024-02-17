#include "evaluator/evaluator.h"
using namespace dt::evaluator;
using namespace dt::execution;

std::shared_ptr<ExecutionPlanNode> Evaluator::eval_float(
        const std::shared_ptr<ast::Float> & node)
{
    return new_float(node->m_value);  // 里面其实调用的是Object::new_float()
}