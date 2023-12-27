#ifndef DTIMEDB_EVALUATOR_H
#define DTIMEDB_EVALUATOR_H

#include <ast/header.h>
using namespace dt::ast;

#include <execution_plan/header.h>
using namespace dt::execution;

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
        bool is_error(const std::shared_ptr<ExecutionPlanNode> & obj);

        std::shared_ptr<ExecutionPlanNode> new_error(const char * format, ...);
        std::shared_ptr<ExecutionPlanNode> new_string(string & value);
        std::shared_ptr<ExecutionPlanNode> new_integer(int64_t value);
        std::shared_ptr<ExecutionPlanNode> new_float(float value);
        std::shared_ptr<ExecutionPlanNode> new_null();

        std::shared_ptr<ExecutionPlanNode> eval(const std::shared_ptr<ast::Node> & node, Environment * env, const std::shared_ptr<ExecutionPlanNode> & root);
        std::shared_ptr<ExecutionPlanNode> eval_program(const std::list<std::shared_ptr<Statement>> & stats, Environment * env, const std::shared_ptr<ExecutionPlanNode> & root);
        std::shared_ptr<ExecutionPlanNode> eval_string(const std::shared_ptr<ast::String> & node);
        std::shared_ptr<ExecutionPlanNode> eval_integer(const std::shared_ptr<ast::Integer> & node);
        std::shared_ptr<ExecutionPlanNode> eval_float(const std::shared_ptr<ast::Float> & node);
        std::shared_ptr<ExecutionPlanNode> eval_null(const std::shared_ptr<ast::Null> & node);
        std::shared_ptr<ExecutionPlanNode> eval_identifier(const std::shared_ptr<ast::Identifier> & node, Environment * env);

        std::shared_ptr<ExecutionPlanNode> eval_use(const std::shared_ptr<Use> & node);
        std::shared_ptr<ExecutionPlanNode> eval_create(const std::shared_ptr<ast::Create> & node);
        std::shared_ptr<ExecutionPlanNode> eval_insert(const std::shared_ptr<ast::Insert> & node);
        std::shared_ptr<ExecutionPlanNode> eval_select(const std::shared_ptr<ast::Select> & node);

        // 前缀表达式求值
        std::shared_ptr<ExecutionPlanNode> eval_prefix(const std::shared_ptr<ast::Prefix> & node, Environment * env, const std::shared_ptr<ExecutionPlanNode> & root);
        std::shared_ptr<ExecutionPlanNode> eval_minus_prefix_expression(const std::shared_ptr<ExecutionPlanNode> & right);

        // 中缀表达式求值
        std::shared_ptr<ExecutionPlanNode> eval_infix(const string & op, const std::shared_ptr<ExecutionPlanNode> & left, const std::shared_ptr<ExecutionPlanNode> & right);
        std::shared_ptr<ExecutionPlanNode> eval_string_infix_expression(const string & op, const std::shared_ptr<ExecutionPlanNode> & left, const std::shared_ptr<ExecutionPlanNode> & right);

    public:

    };
}

#endif //DTIMEDB_EVALUATOR_H
