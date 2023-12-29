#include <evaluator/evaluator.h>
using namespace dt::evaluator;

#include <executor/executor.h>
using namespace dt::executor;

std::shared_ptr<ExecutionPlanNode> Evaluator::eval_use(
        const std::shared_ptr<Use> & node)
{
    std::shared_ptr<UseNode> i(new UseNode());
    i->m_database = node->m_database_name;

    Executor::use_database(node->m_database_name);  // 代替执行计划
    return i;
}