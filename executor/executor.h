#ifndef DTIMEDB_EXECUTOR_H
#define DTIMEDB_EXECUTOR_H

#include <lexer/lexer.h>
using namespace dt::lexer;

#include <parser/parser.h>
using namespace dt::parser;

#include <evaluator/evaluator.h>
using namespace dt::evaluator;

#include <engine/iengine/iengine.h>
using namespace dt::iengine;

#include <engine/tsm/controller.h>
using namespace dt::tsm;

#include <memory>

namespace dt::executor
{
    /**
     * 执行器
     *
     * 解析SQL：将SQL语句解析为抽象语法树（AST）
     * 计划生成：根据AST生成查询或写入计划
     * 执行计划：执行这些计划，这可能涉及调用存储引擎的接口来进行数据检索或更新
     * 返回结果：在查询操作中，执行器处理并返回结果集
     */
    class Executor
    {
    public:

        std::shared_ptr<Program> generate_ast_tree(string & sql);
        void generate_execute_plan(string & sql);
        void execute_plan(const std::shared_ptr<ExecutionPlanNode> & node);

        bool insert();

//        void use_database(string & db_name);
//        string get_database();
    private:
//        string m_current_db;  // 当前操作哪个数据库

        // new saveBankWorker
        Controller * m_tsm = new Controller();  // 引擎控制器
//        std::shared_ptr<Controller> m_clt;

        // BankWork * bw = NULL;
        IEngine * m_engine;  // 引擎接口
    };
}

#endif //DTIMEDB_EXECUTOR_H
