#include "evaluator/evaluator.h"
#include <cstdarg>
using namespace dt::evaluator;
using namespace dt::execution;
using namespace dt::ast;
using std::string;

bool Evaluator::is_error(const std::shared_ptr<ExecutionPlanNode> & obj)
{
    return obj->type() == ExecutionPlanNode::OBJECT_ERROR;
}

std::shared_ptr<ExecutionPlanNode> Evaluator::new_error(const char * format, ...)
{
    char buf[1024] = {0};
    va_list arg_ptr;
    va_start(arg_ptr, format);
    vsnprintf(buf, sizeof(buf), format, arg_ptr);
    va_end(arg_ptr);

    std::shared_ptr<Error> obj = std::make_shared<Error>();
    obj->m_message = buf;
    return obj;
}

std::shared_ptr<ExecutionPlanNode> Evaluator::new_string(string & value)
{
    return ExecutionPlanNode::new_string(value);
}

std::shared_ptr<ExecutionPlanNode> Evaluator::new_integer(int64_t value)
{
    return ExecutionPlanNode::new_integer(value);
}

std::shared_ptr<ExecutionPlanNode> Evaluator::new_float(float value)
{
    return ExecutionPlanNode::new_float(value);
}

std::shared_ptr<ExecutionPlanNode> Evaluator::new_null()
{
    return ExecutionPlanNode::new_null();
}

/**
 * 根据不同节点 返回对应节点
 */
std::shared_ptr<ExecutionPlanNode> Evaluator::eval(
        const std::shared_ptr<ast::Node> & node,
        Environment * env,
        const std::shared_ptr<ExecutionPlanNode> & root)
{
    switch (node->type())
    {
        case Node::NODE_PROGRAM:  // 根结点
        {
            auto s = std::dynamic_pointer_cast<ast::Program>(node);
            return eval_program(s->m_statements, env, root);
        }
        case Node::NODE_EXPRESSION_STATEMENT:  // 表达式语句
        {
            auto s = std::dynamic_pointer_cast<ast::ExpressionStatement>(node);
            return eval(s->m_expression, env, root);  // 递归判断值类型
        }
        case Node::NODE_SYS:  // sys 系统命令处理
        {
            auto sys = std::dynamic_pointer_cast<ast::System>(node);
            std::shared_ptr<ExecutionPlanNode> where;
            if (sys->m_where != nullptr)
            {
                auto e = std::dynamic_pointer_cast<ast::Infix>(sys->m_where);  // 先将类型转换为中缀表达式
                auto left = eval(e->m_left, env, root);  // 对select 的where 的左子树进行求值
                if (is_error(left))
                {
                    return left;
                }
                auto right = eval(e->m_right, env, root);  // 对右子树进行求值
                if (is_error(right))
                {
                    return right;
                }
                where = eval_infix(e->m_operator, left, right);  // 先处理where
            }
            auto s = eval_sys(sys, where);
            root->set_child(s);
            return s;
        }
        case Node::NODE_SHOW:  // show 处理
        {
            auto show = std::dynamic_pointer_cast<ast::Show>(node);
            auto s = eval_show(show);
            root->set_child(s);
            return s;
        }
        case Node::NODE_USE:  // use 处理
        {
            // 该语句不生成执行计划，可以直接进行use 运行
            auto use = std::dynamic_pointer_cast<ast::Use>(node);
            auto u = eval_use(use);
            root->set_child(u);
            return u;
        }
        case Node::NODE_CREATE:  // create 处理
        {
            auto create = std::dynamic_pointer_cast<ast::Create>(node);
            auto c = eval_create(create);
            root->set_child(c);
            return c;
        }
        case Node::NODE_SELECT:  // select 处理
        {
            auto select = std::dynamic_pointer_cast<ast::Select>(node);
            auto e = std::dynamic_pointer_cast<ast::Infix>(select->m_where);  // 先将类型转换为中缀表达式
            auto left = eval(e->m_left, env, root);  // 对select 的where 的左子树进行求值
            if (is_error(left))
            {
                return left;
            }
            auto right = eval(e->m_right, env, root);  // 对右子树进行求值
            if (is_error(right))
            {
                return right;
            }
            auto where = eval_infix(e->m_operator, left, right);  // 先处理where
            return eval_clt_select(select, where, root);
        }
        case Node::NODE_INSERT:  // insert 处理
        {
            auto insert = std::dynamic_pointer_cast<ast::Insert>(node);
            auto child = eval_insert(insert);
            root->set_child(child);
            return child;
        }
        case Node::NODE_STRING:  // string
        {
            auto s = std::dynamic_pointer_cast<ast::String>(node);
            return eval_string(s);
        }
        case Node::NODE_INTEGER:  // integer
        {
            auto s = std::dynamic_pointer_cast<ast::Integer>(node);
            return eval_integer(s);
        }
        case Node::NODE_FLOAT:  // float
        {
            auto s = std::dynamic_pointer_cast<ast::Float>(node);
            return eval_float(s);
        }
        case Node::NODE_INFIX:  // 中缀表达式
        {
            auto s = std::dynamic_pointer_cast<ast::Infix>(node);
            auto left = eval(s->m_left, env, root);
            if (is_error(left))  // 左子树求值
            {
                return left;
            }
            auto right = eval(s->m_right, env, root);
            if (is_error(right))  // 右子树求值
            {
                return right;
            }
            return eval_infix(s->m_operator, left, right);
        }
        case Node::NODE_PREFIX:  // 前缀表达式
        {
            auto s = std::dynamic_pointer_cast<ast::Prefix>(node);
            return eval_prefix(s, env, root);
        }
        case Node::NODE_NULL:  // null
        {
            auto s = std::dynamic_pointer_cast<ast::Null>(node);
            return eval_null(s);
        }
        case Node::NODE_IDENTIFIER:  // 用户定义的变量
        {
            auto s = std::dynamic_pointer_cast<ast::Identifier>(node);
            return eval_identifier(s, env);
        }
        default:
        {
            return new_error("Error : node type error");
        }
    }
}