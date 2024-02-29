#include <execution_plan/operator.h>
using namespace dt::execution;
using namespace dt::impl;
using namespace std::chrono;
using std::string;

void SysNode::execute(IEngine & engine)
{
    if (m_current_db.empty())
    {
        std::cout << "Database not selected, 'use' command" << std::endl;
        return;
    }
    auto * system = dynamic_cast<ISystem*>(&engine);
    if (system == nullptr)
    {
        std::cout << "IEngine -> ISystem 指针转换失败\n";
        return;
    }
    if (m_option == "show")
    {
        system->sys_show_file(m_current_db, m_table_name);
    }
    else if (m_option == "update")
    {
        system->sys_update_file(m_current_db, m_table_name, m_where);
    }
    else if (m_option == "clear")
    {
        system->sys_clear_file(m_current_db, m_table_name);
    }
}

void CreateNode::execute(IEngine & engine)
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
//            engine.create_table();
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

void ShowNode::execute(IEngine & engine)
{
    if (m_current_db.empty())
    {
        std::cout << "Database not selected, 'use' command" << std::endl;
        return;
    }
    engine.show_table(m_current_db);
}

void UseNode::execute(IEngine & engine)
{
    m_current_db = m_database_name;  // 上下文环境
    // 遍历map 加载对应表
    for (auto & tables : m_tb_engine)
    {
        if (engine.load_database(m_database_name, tables.second))  // 加载对应数据库
        {
            std::cout << "use: " << m_database_name << "\n";
        }
    }
}

void ScanNode::execute(IEngine & engine)
{
    if (m_current_db.empty())
    {
        std::cout << "Database not selected, 'use' command" << std::endl;
        return;
    }
    std::cout << "scan : " << m_table_name << std::endl;
//    engine.scan_full_table(m_current_db, m_table_name);
    // 先把指针转换成tsm引擎的,因为tsm引擎的查询是分字段进行的[列存]
    auto * tsm = dynamic_cast<ITSM*>(&engine);
    if (tsm == nullptr)
    {
        std::cout << "IEngine -> ITSM 指针转换失败\n";
        return;
    }
//    tsm->get_range_data(m_current_db);
}

void InsertNode::execute(IEngine & engine)
{
    auto * tsm = dynamic_cast<ITSM*>(&engine);
    if (tsm == nullptr)
    {
        std::cout << "IEngine -> ITSM 指针转换失败\n";
        return;
    }

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
        while ((pos = fv.find('=')) != std::string::npos)
        {
            field = fv.substr(0, pos);
            fv.erase(0, pos + 1);
        }
        value = fv;
        tsm->insert(timestamp, tags_str, value, IEngine::Type::DATA_STRING, field, m_table, m_current_db);
    }

    // 制作内存索引
    tsm->create_index(m_table, m_tags);
}

void QueryTSMNode::execute(IEngine & engine)
{
    auto * tsm = dynamic_cast<ITSM*>(&engine);
    if (tsm == nullptr)
    {
        std::cout << "IEngine -> ITSM 指针转换失败\n";
        return;
    }
    string db_name = m_db_name;
    if (m_db_name.empty())
    {
        if (m_current_db.empty())
        {
            std::cout << "Database not selected, 'use' command" << std::endl;
            return;
        }
        else
        {
            db_name = m_current_db;
        }
    }
    tsm->get_range_data(db_name, m_measurement, m_reduced_fields, m_expr_tree);
}
