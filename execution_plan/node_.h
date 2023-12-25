#ifndef DTIMEDB_NODE_H
#define DTIMEDB_NODE_H

#include <vector>
#include <iostream>
using std::string;

namespace dt::evaluator
{
    /**
     * 执行计划节点
     */
    class PlanNode
    {
    public:
        virtual void generate_plan() = 0;
        virtual void execute() = 0;
        virtual ~PlanNode() {}
    };

    /**
     * 扫描节点
     *
     * 可以是全表扫描或者基于索引的扫描
     *
     * eg:
     * select * from user;
     * 那么执行计划只需要一个ScanNode 来扫描整个表即可
     */
    class ScanNode : public PlanNode
    {
    public:
        ScanNode(const string & table_name) : m_table_name(table_name) {}

        void generate_plan() override
        {
            // 扫描生成计划
        }

        void execute() override
        {
            std::cout << "ScanNode " << m_table_name << std::endl;
            // 真实的扫描表逻辑

        }

    private:
        string m_table_name;
    };

    /**
     * 过滤节点
     *
     * 基于扫描出来的数据，然后只保留满足条件的数据
     *
     * eg：
     * select * from user where age > 30;
     * 执行计划则是包含一个ScanNode 来扫描user，然后是一个FilterNode
     * 来过滤出年龄大于30 岁的
     */
    class FilterNode : public PlanNode
    {
    public:
        FilterNode(const string & condition) : m_condition(condition) {}

        void generate_plan() override
        {
            // 生成过滤计划

            // 递归生成子节点计划
            m_child->generate_plan();
        }

        void execute() override
        {
            std::cout << "FilterNode " << m_condition << std::endl;
            // 过滤逻辑

        }

    private:
        string          m_condition;
        PlanNode *      m_child;
    };

    /**
     * 投影节点
     *
     * 负责实现select 子句中的字段选择，它选择并返回查询结果中所需的列
     */
    class ProjectNode : public PlanNode
    {
    public:
        ProjectNode(const std::vector<string> & fields) : m_fields(fields) {}

        void execute() override
        {
            std::cout << "ProjectNode " << std::endl;
            for (const auto & field : m_fields)
            {
                std::cout << field << " ";
            }
            std::cout << std::endl;
            // 这里可以添加真实的投影逻辑

        }

    private:
        std::vector<string> m_fields;
    };

}

#endif //DTIMEDB_NODE_H
