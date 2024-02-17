#ifndef DTIMEDB_NODE_H
#define DTIMEDB_NODE_H

#include "engine/impl/iclt.h"
#include "engine/impl/iengine.h"
#include "engine/impl/isystem.h"
#include "engine/impl/itsm.h"
#include "token/token.h"

#include <memory>
#include <map>
#include <iostream>
#include <utility>

namespace dt::execution
{
    /**
     * 数据类型基类
     *
     * [被观察者]
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
            OBJECT_SHOW,
            OBJECT_USE,
            OBJECT_SCAN,
            OBJECT_FILTER,
            OBJECT_PROJECT,
            OBJECT_INSERT,
            OBJECT_SYSTEM,

            OBJECT_QUERY_TSM,
        };

        ExecutionPlanNode() {}
        ExecutionPlanNode(Type type) : m_type(type) {}
        virtual ~ExecutionPlanNode() {}

        Type type() const { return m_type; }
        std::string name() const;
        virtual std::string str() const = 0;

        static std::shared_ptr<ExecutionPlanNode> new_error(const char * format, ...);
        static std::shared_ptr<ExecutionPlanNode> new_string(std::string & value);
        static std::shared_ptr<ExecutionPlanNode> new_integer(int64_t value);
        static std::shared_ptr<ExecutionPlanNode> new_float(float value);
        static std::shared_ptr<ExecutionPlanNode> new_null();

        void show_database();

        // 操作符虚函数
        virtual void execute(impl::IEngine & engine) = 0;
        virtual std::shared_ptr<ExecutionPlanNode> get_child() const = 0;
        virtual void set_child(std::shared_ptr<ExecutionPlanNode> child) = 0;

    protected:
        static std::string                       m_current_db;

        Type                                     m_type;
        static std::map<Type, std::string>       m_names;  // 类型 对应字符串名称
    };
}

#endif //DTIMEDB_NODE_H
