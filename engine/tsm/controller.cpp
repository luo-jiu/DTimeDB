#include <engine/tsm/controller.h>
using namespace dt::tsm;

/**
 * 你需要根据自己的引擎进行初始化
 * @param file_path 控制器给你一个新的文件路径
 * @return
 */
string Controller::init_file(
        string & file_path)
{

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
        string & file_path)
{
    if (type == IEngine::Type::DATA_STREAM)
    {
        m_write.write(timestamp, value, DataBlock::DATA_STRING, field_name, file_path);
    }
    else if (type == IEngine::Type::DATA_INTEGER)
    {
        m_write.write(timestamp, value, DataBlock::DATA_INTEGER, field_name, file_path);
    }
    else if (type == IEngine::Type::DATA_FLOAT)
    {
        m_write.write(timestamp, value, DataBlock::Type::DATA_FLOAT, field_name, file_path);
    }
    else
    {
        std::cerr << "Error : Unknown type - " << std::to_string(type) << std::endl;
        return false;
    }
    return true;
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

/**
 * 查询操作
 * @param timestamps
 * @param table_name
 * @param tags
 * @return 结果集 list<string>
 */
std::list<string> Controller::search(
        std::list<high_resolution_clock::time_point> timestamps,
        string & table_name,
        std::list<string> tags)
{

}