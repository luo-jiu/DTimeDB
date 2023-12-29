#ifndef DTIMEDB_NULL_H
#define DTIMEDB_NULL_H

#include <execution_plan/node.h>

namespace dt::execution
{
    class Null : public ExecutionPlanNode
    {
    public:
        Null() : ExecutionPlanNode(OBJECT_NULL) {}
        ~Null() {}

        virtual string str() const { return "null"; }
        void execute(IEngine & engine) override {}
        std::shared_ptr<ExecutionPlanNode> get_child() const override { return nullptr; }
        void set_child(std::shared_ptr<ExecutionPlanNode> child) override {}
    };
}

#endif //DTIMEDB_NULL_H
