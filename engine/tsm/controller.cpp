#include <engine/tsm/controller.h>
using namespace dt::tsm;

/**
 * 创建数据库
 *
 * 其实就是在真实的文件系统上创建了一个文件夹
 */
bool Controller::create_database(
        string & db_name)
{
    auto path = m_file.create_database(db_name);
    if (!path.empty())
    {
        return true;
    }
    return false;
}

/**
 * 确保在该数据库上下文进行操作
 */
bool Controller::use_database(
        string & db_name)
{
    // 将数据加载到内存中
    m_file.load_database(db_name, "tsm");


}

/**
 * 创建表
 * @param tb_name
 * @return
 */
bool Controller::create_table(
        string & tb_name,
        string & db_name)
{
    // 先判断表是否已经存在
    if (m_file.exists_table(tb_name, db_name))
    {
        // 表已经存在
        std::cout << tb_name << " exists" << std::endl;
        return false;
    }

    // 创建表
    return m_file.create_table(tb_name, db_name, "tsm");
}

/**
 * 插入数据操作
 * @param file_path 文件路径
 * @return 操作是否成功
 */
bool Controller::insert(
        high_resolution_clock::time_point timestamp,
        string value,
        Type type,
        string & field_name,
        string & measurement,
        string & db_name)
{
    // 拿取出对应表的writer
    auto db_it = m_map.find(db_name);
    if (db_it != m_map.end())
    {
        auto tb_it = db_it->second.m_table_map.find(measurement);
        if (tb_it != db_it->second.m_table_map.end())
        {
            // 获取字段writer
            auto & writer = tb_it->second;
            DataBlock::Type _type;
            if (type == IEngine::Type::DATA_STREAM)
            {
                _type = DataBlock::DATA_STRING;
            }
            else if (type == IEngine::Type::DATA_INTEGER)
            {
                _type = DataBlock::DATA_INTEGER;
            }
            else if (type == IEngine::Type::DATA_FLOAT)
            {
                _type = DataBlock::DATA_FLOAT;
            }
            else
            {
                std::cerr << "Error : unknown type " << type << std::endl;
                return false;
            }

            writer.m_writer->write(timestamp, value, _type, field_name, db_name);  // 写入
            return true;
        }
    }
    return false;
}

/**
 * 修改操作
 */
bool Controller::update(
        high_resolution_clock::time_point timestamp,
        string value,
        Type type,
        string & table_name)
{

}

bool Controller::get_next_data(string & data)
{

}

void Controller::begin_indexed_scan(
        const high_resolution_clock::time_point & timestamp,
        string & data)
{

}

bool Controller::get_range_datas(
        const high_resolution_clock::time_point & start,
        const high_resolution_clock::time_point & end,
        std::vector<string> & datas)
{

}

bool Controller::get_range_datas(
        std::vector<string> tags,
        std::vector<string> datas)
{

}

