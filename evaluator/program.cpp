#include <evaluator/evaluator.h>
using namespace dt::evaluator;

std::shared_ptr<Object> Evaluator::eval_program(
        const std::list<std::shared_ptr<Statement>> & stats,
        Environment * env)
{
    std::shared_ptr<Object> res;
    for (auto & stat : stats)
    {
        res = eval(stat, env);  // 开始递归

        // 错误则退出
        if (is_error(res))
        {
            break;
        }
    }
    return res;
}