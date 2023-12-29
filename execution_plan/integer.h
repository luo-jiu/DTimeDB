#ifndef DTIMEDB_INTEGER_H
#define DTIMEDB_INTEGER_H

#include <execution_plan/node.h>

namespace dt::execution
{
    class Integer : public ExecutionPlanNode
    {
    public:
        Integer() : ExecutionPlanNode(OBJECT_FLOAT) {}
        Integer(float value) : ExecutionPlanNode(OBJECT_FLOAT), m_value(value) {}
        ~Integer() {}

        virtual string str() const { return std::to_string(m_value); }
        void execute(IEngine & engine) override {}
        std::shared_ptr<ExecutionPlanNode> get_child() const override { return nullptr; }
        void set_child(std::shared_ptr<ExecutionPlanNode> child) override {}

    public:
        int64_t m_value;
    };
}


#endif //DTIMEDB_INTEGER_H
