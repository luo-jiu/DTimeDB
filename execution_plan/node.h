#ifndef DTIMEDB_NODE_H
#define DTIMEDB_NODE_H

#include <engine/iengine/iengine.h>
using namespace dt::iengine;

#include <memory>
#include <map>
#include <iostream>
#include <utility>
using std::string;

namespace dt::execution
{
    /**
     * 数据类型基类
     */
    class ExecutionPlanNode
    {
    public:
        enum Type
        {
            OBJECT_ERROR = 0,
            OBJECT_STRING,
            OBJECT_INTEGER,
            OBJECT_FLOAT,
            OBJECT_NULL,

            // 操作符
            OBJECT_ROOT,
            OBJECT_USE,
            OBJECT_SCAN,
            OBJECT_FILTER,
            OBJECT_PROJECT,
            OBJECT_INSERT,
        };

        ExecutionPlanNode() {}
        ExecutionPlanNode(Type type) : m_type(type) {}
        virtual ~ExecutionPlanNode() {}

        Type type() const { return m_type; }
        string name() const;
        virtual string str() const = 0;

        static std::shared_ptr<ExecutionPlanNode> new_error(const char * format, ...);
        static std::shared_ptr<ExecutionPlanNode> new_string(string & value);
        static std::shared_ptr<ExecutionPlanNode> new_integer(int64_t value);
        static std::shared_ptr<ExecutionPlanNode> new_float(float value);
        static std::shared_ptr<ExecutionPlanNode> new_null();

        // 操作符虚函数
        virtual void execute(IEngine & engine) = 0;
        virtual std::shared_ptr<ExecutionPlanNode> get_child() const = 0;
        virtual void set_child(std::shared_ptr<ExecutionPlanNode> child) = 0;

    protected:
        static string                       m_current_db;

        Type                                m_type;
        static std::map<Type, string>       m_names;  // 类型 对应字符串名称
    };
}

#endif //DTIMEDB_NODE_H
