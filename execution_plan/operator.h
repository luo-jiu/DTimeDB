#ifndef DTIMEDB_OPERATOR_H
#define DTIMEDB_OPERATOR_H

#include <execution_plan/node.h>
#include <file_manager/file_path_manager.h>
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
        string                                 m_engine;  // 表示该执行计划需要在哪个引擎下执行
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
            bool res;
            if (m_type == "database")
            {
                res = engine.create_database(m_name);
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
                    res = engine.create_table(m_name, m_current_db);
                }
            }
            if (res)
            {
                std::cout << "create: " << m_type << ", m_series_key: " << m_name << std::endl;
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
        ShowNode(): ExecutionPlanNode(OBJECT_SHOW) {}
        virtual string str() const { return ""; }
        void execute(IEngine & engine) override
        {
            if (m_current_db.empty())
            {
                std::cout << "Database not selected, 'use' command" << std::endl;
                return;
            }
            engine.show_table(m_current_db);
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
            m_current_db = m_database;  // 上下文环境
            engine.load_database(m_database);  // 加载对应数据库
            std::cout << "use: " << m_database << "\n";
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

        void execute(IEngine & engine) override
        {
            if (m_current_db.empty())
            {
                std::cout << "Database not selected, 'use' command" << std::endl;
                return;
            }
            std::cout << "scan : " << m_table_name << std::endl;
            engine.scan_full_table(m_current_db, m_table_name);
        }

        std::shared_ptr<ExecutionPlanNode> get_child() const override { return m_child; }
        void set_child(std::shared_ptr<ExecutionPlanNode> child) { m_child = child; }

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
        std::list<string>                       m_fields;
        std::map<Token::Type, string>           m_where;
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
            nanoseconds nanoseconds(nanoseconds_count);
            high_resolution_clock::time_point timestamp(duration_cast<high_resolution_clock::duration>(nanoseconds));

            // 对tags 进行排序
            m_tags.sort();
            string tags_str;
            for (const auto& tag : m_tags)
            {
                tags_str += tag;
            }

            // 调用存储引擎接口的插入函数
            for (string fv : m_fv)
            {
                size_t pos;
                string field, value;
                while ((pos = fv.find('=')) != std::string::npos) {
                    field = fv.substr(0, pos);
                    fv.erase(0, pos + 1);
                }
                value = fv;
                engine.insert(timestamp, tags_str, value, IEngine::Type::DATA_STRING, field, m_table, m_current_db);
            }

            // 制作内存索引
            engine.create_index(m_table, m_tags);
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
