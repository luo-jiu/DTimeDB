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
     * 需要获取执行计划
     * 然后根据执行计划进行函数调用
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
