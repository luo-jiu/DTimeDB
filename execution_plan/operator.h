#ifndef DTIMEDB_OPERATOR_H
#define DTIMEDB_OPERATOR_H

#include <execution_plan/node.h>
#include <engine/file_manager/file_path_manager.h>
using namespace dt::file;

#include <chrono>
using namespace std::chrono;

#include <iostream>
#include <utility>
using std::string;

namespace dt::execution
{
    class RootNode : public ExecutionPlanNode
    {
    public:
        RootNode() : ExecutionPlanNode(OBJECT_ROOT) {}

        virtual string str() const { return ""; }
        void execute(IEngine & engine) override {}
        std::shared_ptr<ExecutionPlanNode> get_child() const override { return m_child; }
        void set_child(std::shared_ptr<ExecutionPlanNode> child) override { m_child = child; }

    public:
        std::shared_ptr<ExecutionPlanNode>     m_child;
    };

    /**
     * 创建节点
     */
    class CreateNode : public ExecutionPlanNode
    {
    public:
        virtual string str() const { return ""; }
        void execute(IEngine & engine) override
        {
            std::cout << "create: " << m_type << ", m_name: " << m_name << std::endl;
            if (m_type == "database")
            {
                engine.create_database(m_name);
            }
            else if (m_type == "table")
            {
                // 表才区分引擎
                if (m_engine == "clt")
                {
//                    engine.create_table();
                }
                else  // 默认tsm 引擎
                {
                    engine.create_table(m_name, m_current_db);
                }

            }
        }
        std::shared_ptr<ExecutionPlanNode> get_child() const override
        {
            return m_child;
        }
        void set_child(std::shared_ptr<ExecutionPlanNode> child) override {}

    public:
        string                                 m_type;    // database or table
        string                                 m_engine;  // 引擎
        std::list<string>                      m_field;   // 字段[clt引擎需要]
        string                                 m_name;
        std::shared_ptr<ExecutionPlanNode>     m_child;
    };

    /**
     * 显示信息
     */
    class ShowNode : public ExecutionPlanNode
    {
    public:
        virtual string str() const { return ""; }
        void execute(IEngine & engine) override
        {
//            std::cout << "show: " << m_type << std::endl;

            // 遍历数据库 或者 表
            if (m_type == "database")
            {
                FilePathManager::show_data("");
            }
            else
            {
                if (m_current_db.empty())
                {
                    std::cout << "Database not selected, 'use' command" << std::endl;
                    return;
                }
                FilePathManager::show_data(m_current_db);
            }
        }
        std::shared_ptr<ExecutionPlanNode> get_child() const override { return m_child; }
        void set_child(std::shared_ptr<ExecutionPlanNode> child) { m_child = child; }

    public:
        string                                  m_type;
        std::shared_ptr<ExecutionPlanNode>      m_child;
    };

    /**
     * 选择数据库
     */
    class UseNode : public ExecutionPlanNode
    {
    public:
        UseNode() : ExecutionPlanNode(OBJECT_USE) {}

        virtual string str() const { return ""; }
        void execute(IEngine & engine) override
        {
            std::cout << "use: " << m_database << std::endl;
            m_current_db = m_database;
            engine.use_database(m_database);
        }
        std::shared_ptr<ExecutionPlanNode> get_child() const override
        {
            return m_child;
        }
        void set_child(std::shared_ptr<ExecutionPlanNode> child) override {}

    public:
        string                                 m_database;
        std::shared_ptr<ExecutionPlanNode>     m_child;
    };

    /**
     * 扫描操作符
     */
    class ScanNode : public ExecutionPlanNode
    {
    public:
        enum Type
        {
            SCAN_INDEXES,
            SCAN_FULL_TABLE
        };

        ScanNode() : ExecutionPlanNode(OBJECT_SCAN) {}
        ScanNode(const string & table_name) : ExecutionPlanNode(OBJECT_SCAN), m_table_name(table_name) {}
        ~ScanNode() {}

        virtual string str() const { return m_table_name; }
        void execute(IEngine & engine) override { std::cout << "scan : " << m_table_name << std::endl; }
        std::shared_ptr<ExecutionPlanNode> get_child() const override { return m_child; }
        void set_child(std::shared_ptr<ExecutionPlanNode> & child) { m_child = child; }

    public:
        string                                 m_table_name;
        Type                                   m_scan_type;   // 全表扫描 | 索引扫描
        int                                    m_row_limit;   // 限制返回的最大行数
        std::shared_ptr<ExecutionPlanNode>     m_child;
    };

    /**
     * 过滤节点
     */
    class FilterNode : public ExecutionPlanNode
    {
    public:
        FilterNode() : ExecutionPlanNode(OBJECT_FILTER) {}
        FilterNode(const string & condition) : m_condition(condition), ExecutionPlanNode(OBJECT_FILTER) {}

        virtual string str() const { return ""; }
        void execute(IEngine & engine) override { std::cout << "filter : " << std::endl; }
        std::shared_ptr<ExecutionPlanNode> get_child() const override { return m_child; }
        void set_child(std::shared_ptr<ExecutionPlanNode> child) override { m_child = child; }

    public:
        string                                  m_condition;
        std::shared_ptr<ExecutionPlanNode>      m_child;
    };

    /**
     * 投影节点
     */
    class ProjectNode : public ExecutionPlanNode
    {
    public:
        virtual string str() const { return ""; }
        void execute(IEngine & engine) override { std::cout << "project : " << std::endl; }
        std::shared_ptr<ExecutionPlanNode> get_child() const override { return m_child; }
        void set_child(std::shared_ptr<ExecutionPlanNode> child) override { m_child = child; }

    public:
        std::shared_ptr<ExecutionPlanNode>      m_child;
    };

    /**
     * 插入操作符
     */
    class InsertNode : public ExecutionPlanNode
    {
    public:
        InsertNode() : ExecutionPlanNode(OBJECT_INSERT) {}
        ~InsertNode() {}

        enum Type
        {
            ENGINE_TSM,
            ENGINE_CLT
        };

        void set_type(Type type) { m_type = type; }
        void set_table(const string & table) { m_table = table; }
        void set_timestamp(const string & timestamp) { m_timestamp = timestamp; }
        void add_tag(string & tag) { m_tags.push_back(tag); }
        void add_field(string & fv) { m_fv.push_back(fv); }

        virtual string str() const {return ""; }
        // 执行任务
        void execute(IEngine & engine) override
        {
            // 先给时间戳转类型
            long long nanoseconds_count = std::stoll(m_timestamp);
            std::chrono::nanoseconds nanoseconds(nanoseconds_count);
            std::chrono::high_resolution_clock::time_point timestamp(std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(nanoseconds));

            // 调用存储引擎接口的插入函数
            for (string fv : m_fv)
            {
                size_t pos = 0;
                string field, value;
                while ((pos = fv.find('=')) != std::string::npos) {
                    field = fv.substr(0, pos);
                    fv.erase(0, pos + 1);
                }
                value = fv;
                string db = "fv";
                engine.insert(timestamp, value, IEngine::Type::DATA_STREAM, field, m_table, db);
            }
        }
        std::shared_ptr<ExecutionPlanNode> get_child() const override { return m_child; }
        void set_child(std::shared_ptr<ExecutionPlanNode> child) override { m_child = child; }

    public:
        Type                                    m_type;  // 引擎类型
        string                                  m_table;  // 目标表

        std::list<string>                       m_tags;
        std::list<string>                       m_fv;
        string                                  m_timestamp;
        std::shared_ptr<ExecutionPlanNode>      m_child;
    };
}

#endif //DTIMEDB_OPERATOR_H
