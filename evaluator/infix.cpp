#include "evaluator/evaluator.h"
using namespace dt::evaluator;
using namespace dt::execution;
using std::string;

/**
 * 中缀表达式求值
 *
 * 这里只会是where 条件下的
 */
std::shared_ptr<ExecutionPlanNode> Evaluator::eval_infix(
        const string & op,
        const std::shared_ptr<ExecutionPlanNode> & left,
        const std::shared_ptr<ExecutionPlanNode> & right)
{
    switch (left->type())
    {
        case ExecutionPlanNode::OBJECT_STRING:  // 左边类型: string
        {
            if (right->type() == ExecutionPlanNode::OBJECT_STRING)  //  要么tag 要么 字段=值
            {
                // 两边都是字符串，直接将key=value 拼接返回
                return eval_string_infix_expression(op, left, right);
            }
            else
            {
//                return eval_string_infix_expression(op, left, right);
            }
        }
        case ExecutionPlanNode::OBJECT_INTEGER:  // integer
        {

        }
        case ExecutionPlanNode::OBJECT_FLOAT:
        {

        }
        case ExecutionPlanNode::OBJECT_NULL:
        {

        }
        default:
        {
            break;  // 其他一率无法解析
        }
    }
    return new_error("unknown operation: %s %s %s", left->name().c_str(), op.c_str(), right->name().c_str());
}

std::shared_ptr<ExecutionPlanNode> Evaluator::eval_string_infix_expression(
        const string & op,
        const std::shared_ptr<ExecutionPlanNode> & left,
        const std::shared_ptr<ExecutionPlanNode> & right)
{
    auto l = std::dynamic_pointer_cast<execution::String>(left);
    auto r = std::dynamic_pointer_cast<execution::String>(right);
    if (op == "=")
    {
        string kv = l->str() + "=" + r->str();
        return new_string(kv);
    }
    else if (op == "<=")
    {
        string kv = l->str() + "<=" + r->str();
        return new_string(kv);
    }
    else if (op == ">=")
    {
        string kv = l->str() + ">=" + r->str();
        return new_string(kv);
    }
    else if (op == "<")
    {
        string kv = l->str() + "<" + r->str();
        return new_string(kv);
    }
    else if (op == ">")
    {
        string kv = l->str() + ">" + r->str();
        return new_string(kv);
    }
    else if (op == "and")
    {
        string kv = l->str() + "&" + r->str();
        return new_string(kv);
    }
    else if (op == "or")
    {
        string kv = l->str() + "|" + r->str();
        return new_string(kv);
    }
    else  // 字符串之前不支持其他符号运算
    {
        return new_error("unknown operator: %s %s %s", left->name().c_str(), op.c_str(), right->name().c_str());
    }
}