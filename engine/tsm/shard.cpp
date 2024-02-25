#include <iomanip>
#include "shard.h"
using namespace dt::tsm;

Shard::Shard(
        const std::string & databases,
        const std::string & measurement,
        std::time_t timestamp)
{
    m_db_name = databases;
    m_mea_name = measurement;
    m_time_range = get_day_range(timestamp);
}

std::pair<std::time_t, std::time_t> Shard::get_day_range(
        std::time_t timestamp,
        int days)
{
    // 转换时间戳到std::tm结构体，表示本地时间
    std::tm* tm_local = std::localtime(&timestamp);

    // 调整tm结构体到当天的开始（0点0分0秒）
    tm_local->tm_hour = 0;
    tm_local->tm_min = 0;
    tm_local->tm_sec = 0;

    // 转换回时间戳，得到当天开始的时间戳
    std::time_t start_timestamp = std::mktime(tm_local);

    // 计算结束时间的时间戳（在开始时间的基础上加上(days * 24小时 - 1秒)）
    std::time_t end_timestamp = start_timestamp + days * 24 * 60 * 60 - 1;

    return {start_timestamp, end_timestamp};
}

/**
 * 通过时间戳计算shard key
 */
std::string Shard::timestamp_to_shard_key(
        std::time_t timestamp)
{
    std::tm* tm = std::localtime(&timestamp);
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d");
    return oss.str();
}

std::time_t Shard::shard_key_to_timestamp(const std::string & shard_id)
{
    std::tm tm = {};
    std::istringstream ss(shard_id);

    // 使用std::get_time来从字符串解析时间，注意格式需要与timestamp_to_shard_key函数中的输出格式匹配
    ss >> std::get_time(&tm, "%Y-%m-%d");

    // 检查是否成功解析
    if(ss.fail()) {
        throw std::runtime_error("Failed to parse the shard key");
    }

    // 将std::tm转换为std::time_t
    // 注意：这里假设tm表示的是本地时间
    std::time_t timestamp = std::mktime(&tm);

    // 检查转换结果
    if (timestamp == -1) {
        throw std::runtime_error("Conversion to time_t failed");
    }

    return timestamp;
}

/**
 * 序列化
 */
std::string Shard::serialize()
{

}

/**
 * 反序列化
 */
void Shard::deserialize()
{

}

void Shard::update_file_point_offset(const std::string & point_type, int64_t offset)
{
    if (point_type == "head")
    {
        m_curr_tsm_head_offset = offset;
    }
    else if (point_type == "tail")
    {
        m_curr_tsm_tail_offset = offset;
    }
}

void Shard::update_file_margin(int64_t margin)
{
    m_curr_tsm_margin = margin;
}

void Shard::create_tsm_file(const std::string & new_tsm_file_path, int64_t margin)
{
    m_curr_tsm_head_offset = 5;
    m_curr_tsm_tail_offset = margin;
    m_curr_tsm_margin = margin;
    m_curr_tsm_file_path = new_tsm_file_path;

    const std::string & file_path = new_tsm_file_path;
    size_t last_slash_pos = file_path.find_last_of('/');
    std::string file_name;
    if (last_slash_pos != std::string::npos)
    {
        file_name = file_path.substr(last_slash_pos + 1);
    }
    else
    {
        file_name = file_path;
    }
    m_tsm_files.push_back(file_name);
}