#ifndef DTIMEDB_EXECUTOR_H
#define DTIMEDB_EXECUTOR_H

#include "evaluator/evaluator.h"
#include "engine/impl/iclt.h"
#include "engine/impl/iengine.h"
#include "engine/impl/isystem.h"
#include "engine/impl/itsm.h"
#include "engine/tsm/controller.h"

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
        Executor()
        {
            // 为多个引擎的调用提供映射
            m_engine_map["tsm"] = std::make_shared<tsm::Controller>();
        }
        void execute_plan(const std::shared_ptr<execution::ExecutionPlanNode>& node);

    private:
        std::map<std::string, std::shared_ptr<IEngine>> m_engine_map;

//        Controller* m_tsm = new Controller();  // 引擎控制器
//        IEngine* m_engine;  // 引擎接口
    };
}

#endif //DTIMEDB_EXECUTOR_H
