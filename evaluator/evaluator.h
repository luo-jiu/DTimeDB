#ifndef DTIMEDB_EVALUATOR_H
#define DTIMEDB_EVALUATOR_H

#include <ast/header.h>
using namespace dt::ast;

#include <object/header.h>
using namespace dt::object;

#include <evaluator/environment.h>

#include <memory>

namespace dt::evaluator
{
    /**
     * 递归求值生成 执行计划树
     *
     */
    class Evaluator
    {
    public:
        bool is_error(const std::shared_ptr<Object> & obj);

        std::shared_ptr<Object> new_error(const char * format, ...);
        std::shared_ptr<Object> new_string(string & value);
        std::shared_ptr<Object> new_integer(int64_t value);
        std::shared_ptr<Object> new_float(float value);
        std::shared_ptr<Object> new_null();

        std::shared_ptr<Object> eval(const std::shared_ptr<ast::Node> & node, Environment * env);
        std::shared_ptr<Object> eval_program(const std::list<std::shared_ptr<Statement>> & stats, Environment * env);
        std::shared_ptr<Object> eval_string(const std::shared_ptr<ast::String> & node);
        std::shared_ptr<Object> eval_integer(const std::shared_ptr<ast::Integer> & node);
        std::shared_ptr<Object> eval_float(const std::shared_ptr<ast::Float> & node);
        std::shared_ptr<Object> eval_null(const std::shared_ptr<ast::Null> & node);
        std::shared_ptr<Object> eval_identifier(const std::shared_ptr<ast::Identifier> & node, Environment * env);

        // 前缀表达式求值
        std::shared_ptr<Object> eval_prefix(const std::shared_ptr<ast::Prefix> & node, Environment * env);
        std::shared_ptr<Object> eval_minus_prefix_expression(const std::shared_ptr<Object> & right);

        // 中缀表达式求值
        std::shared_ptr<Object> eval_infix(const string & op, const std::shared_ptr<Object> & left, const std::shared_ptr<Object> & right);

    };
}

#endif //DTIMEDB_EVALUATOR_H
