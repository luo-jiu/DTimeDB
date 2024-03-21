#include "clt_controller.h"
using namespace dt::clt;
using std::string;

Controller::Controller(): m_file("clt")
{

}

/**
 * 加载该数据库表信息
 */
bool Controller::load_database(
        std::string & db_name,
        std::vector<std::string> & tables)
{
    std::unique_lock<std::shared_mutex> lock(m_database_mutex);
    for (auto& tb_name : tables)
    {
        bool exist = false;
        auto db_it = m_databases.find(db_name);
        if (db_it != m_databases.end())
        {
            auto tb_it = db_it->second.m_tables.find(tb_name);
            if (tb_it != db_it->second.m_tables.end())
            {
                // 说明表存在
                exist = true;
            }
        }

        if (!exist)
        {
            m_databases[db_name].m_tables[tb_name].m_fields = m_file.load_mea_fields(db_name, tb_name);
        }
    }
    lock.unlock();

    return m_file.load_database(db_name, tables);
}

bool Controller::create_database(
        string & db_name)
{
    // 不创建
    return false;
}

bool Controller::create_table(
        std::list<std::pair<std::string, std::string>> & fields,
        std::string db_name,
        std::string tb_name)
{
    if (m_file.exists_table(db_name, tb_name, false))
    {
        // 表已经存在
        std::cout << tb_name << " exists" << std::endl;
        return false;
    }

    // 创建表
    m_file.create_table(tb_name, db_name);
    for (auto& field : fields)
    {
        m_file.insert_field_to_file(db_name, tb_name, field.first + "=" + field.second);
    }

    return true;
}

void Controller::show_table(std::string & db_name)
{
    m_file.show_data(db_name);
}