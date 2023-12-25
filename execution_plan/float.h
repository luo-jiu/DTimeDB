#ifndef DTIMEDB_FLOAT_H
#define DTIMEDB_FLOAT_H

#include <execution_plan/node.h>

namespace dt::execution
{
    class Float : public ExecutionPlanNode
    {
    public:
        Float() : ExecutionPlanNode(OBJECT_FLOAT) {}
        Float(float value) : ExecutionPlanNode(OBJECT_FLOAT), m_value(value) {}
        ~Float() {}

        virtual string str() const { return std::to_string(m_value); }
        void execute(std::shared_ptr<IEngine> engine) override {}
        std::shared_ptr<ExecutionPlanNode> get_child() const override { return nullptr; }
        void set_child(std::shared_ptr<ExecutionPlanNode> child) override {}

    public:
        float m_value;
    };
}

#endif //DTIMEDB_FLOAT_H
