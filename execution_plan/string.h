#ifndef DTIMEDB_STRING_H
#define DTIMEDB_STRING_H

#include "execution_plan/node.h"

namespace dt::execution
{
    /**
     * 字符串类型
     */
    class String : public ExecutionPlanNode
    {
    public:
        String() : ExecutionPlanNode(OBJECT_STRING) {}
        String(std::string value) : ExecutionPlanNode(OBJECT_STRING), m_value(value) {}
        ~String() {}

        virtual std::string str() const { return m_value; }
        void execute(impl::IEngine & engine) override {}
        std::shared_ptr<ExecutionPlanNode> get_child() const override { return nullptr; }
        void set_child(std::shared_ptr<ExecutionPlanNode> child) override {}

    public:
        std::string m_value;
    };
}

#endif //DTIMEDB_STRING_H
