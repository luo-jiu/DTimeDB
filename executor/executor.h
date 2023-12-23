#ifndef DTIMEDB_EXECUTOR_H
#define DTIMEDB_EXECUTOR_H

#include <engine/iengine/iengine.h>
using namespace dt::iengine;

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


    private:



        bool insert();

    private:
        std::unique_ptr<IEngine> m_engine;
    };
}

#endif //DTIMEDB_EXECUTOR_H
