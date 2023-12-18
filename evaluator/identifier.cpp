#include <evaluator/evaluator.h>
using namespace dt::evaluator;

std::shared_ptr<Object> Evaluator::eval_identifier(
        const std::shared_ptr<ast::Identifier> & node,
        Environment * env)
{
    auto obj = env->get(node->m_value);  // 通过变量名获取值
    if (obj)
    {
        return obj;
    }
    return new_error("identifier not found: %s", node->m_value.c_str());
}