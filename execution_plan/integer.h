#ifndef DTIMEDB_INTEGRE_H
#define DTIMEDB_INTEGRE_H

#include <execution_plan/node.h>

namespace dt::execution
{
    /**
     * 整形类型
     */
    class Integer : public ExecutionPlanNode
    {
    public:
        Integer() : ExecutionPlanNode(OBJECT_INTEGER) {}
        Integer(int64_t value) : ExecutionPlanNode(OBJECT_INTEGER), m_value(value) {}
        ~Integer() {}

        virtual string str() const { return std::to_string(m_value); }
        void execute(std::shared_ptr<IEngine> engine) override {}
        std::shared_ptr<ExecutionPlanNode> get_child() const override { return nullptr; }
        void set_child(std::shared_ptr<ExecutionPlanNode> child) override {}

    public:
        int64_t m_value;
    };
}

#endif //DTIMEDB_INTEGRE_H
