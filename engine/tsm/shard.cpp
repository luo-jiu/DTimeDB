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