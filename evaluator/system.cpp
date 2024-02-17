#include "evaluator/evaluator.h"
using namespace dt::evaluator;
using namespace dt::execution;

std::shared_ptr<ExecutionPlanNode> Evaluator::eval_sys(
        const std::shared_ptr<ast::System> & node,
        const std::shared_ptr<ExecutionPlanNode> & where)
{
    std::shared_ptr<SysNode> s(new SysNode());
    s->m_option = node->m_option;
    s->m_table_name = node->m_from;
    if (where != nullptr)
    {
        s->m_where = where->str();
    }
    return s;
}