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
            root->m_engine = c->m_engine;  // 设置引擎
            root->set_child(c);
            return c;
        }
        case Node::NODE_SELECT:  // select 处理
        {
            auto select = std::dynamic_pointer_cast<ast::Select>(node);
            return eval_select(select, root);
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

bool Evaluator::use_database(const std::string & db_name)
{
    if (m_tab_eng_manager.database_exist(db_name))
    {
        m_curr_db_name = db_name;
        return true;
    }
    return false;
}

/**
 * 递归将查询语句的where ast抽象语法树转换为表达式树
 */
std::shared_ptr<ExprNode> Evaluator::convert_to_new_tree(
        const std::shared_ptr<ast::Expression> & old_node)
{
    if (!old_node)
    {
        // 当前节点为空则返回
        return nullptr;
    }
    // 基于old_node 创建表达式树节点
    auto new_node = std::make_shared<ExprNode>();

    if (old_node->m_token.type() == token::Token::TOKEN_STRING)
    {
        // 说明到叶子节点了, 不会再有子节点了, 程序返回
        auto new_string = std::dynamic_pointer_cast<ast::String>(old_node);
        new_node->m_val = new_string->m_value;
        return new_node;
    }
    else
    {
        auto new_infix = std::dynamic_pointer_cast<ast::Infix>(old_node);
        // 继续递归
        new_node->m_val = new_infix->m_operator;
        if (new_infix->m_left->m_token.type() == token::Token::TOKEN_STRING)
        {
            new_node->m_left = convert_to_new_tree(std::dynamic_pointer_cast<ast::String>(new_infix->m_left));
        }
        else
        {
            new_node->m_left = convert_to_new_tree(std::dynamic_pointer_cast<ast::Infix>(new_infix->m_left));
        }

        if (new_infix->m_right->m_token.type() == token::Token::TOKEN_STRING)
        {
            new_node->m_right = convert_to_new_tree(std::dynamic_pointer_cast<ast::String>(new_infix->m_right));
        }
        else
        {
            new_node->m_right = convert_to_new_tree(std::dynamic_pointer_cast<ast::Infix>(new_infix->m_right));
        }
    }
    return new_node;
}
