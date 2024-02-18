#ifndef DTIMEDB_EVALUATOR_H
#define DTIMEDB_EVALUATOR_H

#include "engine/impl/itsm.h"
#include "ast/header.h"
#include "execution_plan/header.h"
#include "evaluator/environment.h"
#include "file_manager/table_engine_map.h"

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
        bool is_error(const std::shared_ptr<execution::ExecutionPlanNode> & obj);

        std::shared_ptr<execution::ExecutionPlanNode> new_error(const char * format, ...);
        std::shared_ptr<execution::ExecutionPlanNode> new_string(std::string & value);
        std::shared_ptr<execution::ExecutionPlanNode> new_integer(int64_t value);
        std::shared_ptr<execution::ExecutionPlanNode> new_float(float value);
        std::shared_ptr<execution::ExecutionPlanNode> new_null();

        std::shared_ptr<execution::ExecutionPlanNode> eval(const std::shared_ptr<ast::Node> & node, Environment * env, const std::shared_ptr<execution::ExecutionPlanNode> & root);
        std::shared_ptr<execution::ExecutionPlanNode> eval_program(const std::list<std::shared_ptr<dt::ast::Statement>> & stats, Environment * env, const std::shared_ptr<execution::ExecutionPlanNode> & root);
        std::shared_ptr<execution::ExecutionPlanNode> eval_string(const std::shared_ptr<ast::String> & node);
        std::shared_ptr<execution::ExecutionPlanNode> eval_integer(const std::shared_ptr<ast::Integer> & node);
        std::shared_ptr<execution::ExecutionPlanNode> eval_float(const std::shared_ptr<ast::Float> & node);
        std::shared_ptr<execution::ExecutionPlanNode> eval_null(const std::shared_ptr<ast::Null> & node);
        std::shared_ptr<execution::ExecutionPlanNode> eval_identifier(const std::shared_ptr<ast::Identifier> & node, Environment * env);

        std::shared_ptr<execution::ExecutionPlanNode> eval_show(const std::shared_ptr<ast::Show> & node);
        std::shared_ptr<execution::ExecutionPlanNode> eval_use(const std::shared_ptr<dt::ast::Use> & node);
        std::shared_ptr<execution::ExecutionPlanNode> eval_create(const std::shared_ptr<ast::Create> & node);
        std::shared_ptr<execution::ExecutionPlanNode> eval_insert(const std::shared_ptr<ast::Insert> & node);
        std::shared_ptr<execution::ExecutionPlanNode> eval_select(const std::shared_ptr<ast::Select> & select, const std::shared_ptr<execution::ExecutionPlanNode> & root);

        std::shared_ptr<execution::ExecutionPlanNode> eval_sys(const std::shared_ptr<ast::System> & node, const std::shared_ptr<execution::ExecutionPlanNode> & where);

        // 前缀表达式求值
        std::shared_ptr<execution::ExecutionPlanNode> eval_prefix(const std::shared_ptr<ast::Prefix> & node, Environment * env, const std::shared_ptr<execution::ExecutionPlanNode> & root);
        std::shared_ptr<execution::ExecutionPlanNode> eval_minus_prefix_expression(const std::shared_ptr<execution::ExecutionPlanNode> & right);

        // 中缀表达式求值
        std::shared_ptr<execution::ExecutionPlanNode> eval_infix(const std::string & op, const std::shared_ptr<execution::ExecutionPlanNode> & left, const std::shared_ptr<execution::ExecutionPlanNode> & right);
        std::shared_ptr<execution::ExecutionPlanNode> eval_string_infix_expression(const std::string & op, const std::shared_ptr<execution::ExecutionPlanNode> & left, const std::shared_ptr<execution::ExecutionPlanNode> & right);

        bool use_database(const std::string & db_name);
        // 转换成表达式树
        std::shared_ptr<ExprNode> convert_to_new_tree(const std::shared_ptr<ast::Expression> & old_node);

    public:
        std::string                     m_curr_db_name;
        file::TableEngineManager        m_tab_eng_manager;
    };
}

#endif //DTIMEDB_EVALUATOR_H
