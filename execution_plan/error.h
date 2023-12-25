#ifndef DTIMEDB_ERROR_H
#define DTIMEDB_ERROR_H

#include <execution_plan/node.h>

namespace dt::execution
{
    class Error : public ExecutionPlanNode
    {
    public:
        Error() : ExecutionPlanNode(OBJECT_ERROR) {}
        Error(const string & message) : ExecutionPlanNode(OBJECT_ERROR), m_message(message) {}
        ~Error() {}

        virtual string str() const { return m_message; }
        void execute(std::shared_ptr<IEngine> engine) override {}
        std::shared_ptr<ExecutionPlanNode> get_child() const override { return nullptr; }
        void set_child(std::shared_ptr<ExecutionPlanNode> child) override {}

    public:
        string m_message;
    };
}

#endif //DTIMEDB_ERROR_H
