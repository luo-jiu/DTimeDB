#include "evaluator/evaluator.h"
using namespace dt::evaluator;
using namespace dt::execution;
using namespace dt::ast;

std::shared_ptr<ExecutionPlanNode> Evaluator::eval_program(
        const std::list<std::shared_ptr<Statement>> & stats,
        Environment * env,
        const std::shared_ptr<ExecutionPlanNode> & root)
{
    std::shared_ptr<ExecutionPlanNode> res;
    for (auto & stat : stats)
    {
        /**
         * 这里是第一次调用eval 进入递归求值
         *
         * 需要传入执行计划的根结点进行执行计划的生成
         */

        res = eval(stat, env, root);  // 开始递归

        // 错误则退出
        if (is_error(res))
        {
            break;
        }
    }
    return res;
}