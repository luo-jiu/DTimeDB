#include <evaluator/evaluator.h>
using namespace dt::evaluator;

std::shared_ptr<ExecutionPlanNode> Evaluator::eval_show(
        const std::shared_ptr<ast::Show> & node)
{
    std::shared_ptr<ShowNode> i(new ShowNode());
    i->m_type = node->m_type;
    return i;
}