#include <evaluator/evaluator.h>

using namespace dt::evaluator;

/**
 * 前缀表达式求值
 */
std::shared_ptr<Object> Evaluator::eval_prefix(
        const std::shared_ptr<ast::Prefix> & node,
        Environment * env)
{
    string op = node->m_operator;
    auto right = eval(node->m_right, env);
    if (is_error(right))
    {
        return right;
    }

    // 暂时只有负号
    if (op == "-")
    {
        return eval_minus_prefix_expression(right);
    }
    else
    {
        return new_error("Error : unknown operator : %s%s", op.c_str(), right->name().c_str());
    }
}

/**
 * 对'-' 求值
 */
std::shared_ptr<Object> Evaluator::eval_minus_prefix_expression(
        const std::shared_ptr<Object> & right)
{
    switch (right->type())
    {
        case Object::OBJECT_INTEGER:
        {
            auto r = std::dynamic_pointer_cast<object::Integer>(right);
            return new_integer(-(r->m_value));
        }
        default:
        {
            return new_error("Error : unknown operator: -%s", right->name().c_str());
        }
    }
}
