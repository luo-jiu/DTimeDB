#include <engine/tsm/controller.h>
using namespace dt::tsm;

/**
 * 插入数据操作
 * @param file_path 文件路径
 * @return 操作是否成功
 */
bool Controller::insert(
        high_resolution_clock::time_point timestamp,
        string value,
        Type type,
        string & field_name)
{

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

