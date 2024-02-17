#ifndef DTIMEDB_ENVIRONMENT_H
#define DTIMEDB_ENVIRONMENT_H

#include "execution_plan/node.h"

namespace dt::evaluator
{
    class Environment
    {
    public:
        Environment(): m_outer(nullptr) {}
        Environment(Environment * outer): m_outer(outer) {}
        ~Environment() {}

        /**
         * 存放变量名和值
         */
        void set(const std::string & name, const std::shared_ptr<dt::execution::ExecutionPlanNode> & value)
        {
            m_store[name] = value;
        }

        /**
         * 通过变量名获取变量的值
         */
        std::shared_ptr<dt::execution::ExecutionPlanNode> get(const std::string & name)
        {
            auto it = m_store.find(name);
            if (it != m_store.end())
            {
                return it->second;
            }
            if (m_outer)
            {
                return m_outer->get(name);  // 查看上层变量有没有
            }
            return nullptr;
        }

        /**
         * 判断一个变量是否存在
         */
        bool has(const std::string & name)
        {
            auto it = m_store.find(name);
            if (it != m_store.end())
            {
                return true;
            }
            else if (m_outer)
            {
                m_outer->has(name);
            }
            return false;
        }

        /**
         * 删除变量(只删除当前环境的)
         */
        void remove(const std::string & name)
        {
            auto it = m_store.find(name);
            if (it != m_store.end())
            {
                m_store.erase(it);
            }
        }

        /**
         * 清楚所有变量
         */
        void clear()
        {
            m_store.clear();
        }

    private:
        //       变量名        变量值
        std::map<std::string, std::shared_ptr<dt::execution::ExecutionPlanNode>>      m_store;
        Environment *                                       m_outer;  // 外层环境变量

    };
}

#endif //DTIMEDB_ENVIRONMENT_H
