#include <engine/tsm/index.h>
using namespace dt::tsm;

/**
 * 构建内存中索引
 */
void Index::create_index(
        const string & measurement,
        std::list<string> & tags)
{
    // 先拼接series_key
    string series_key = measurement;
    tags.sort();
    for (const auto& tag : tags)
    {
        series_key += tag;
    }
    std::cout << "series_key:" << series_key << "\n";

    if (series_key_empty(series_key))
    {
        std::cout << "series_key不存在,准备添加到倒排索引\n";
        SeriesKey key{series_key};
        m_series_block.m_series_key_chunks.insert(key);
    }
    else
    {
        std::cout << "series_key存在,已忽略\n";
    }
}

/**
 * 判断seriesKey 是否存在
 */
bool Index::series_key_empty(
        string & series_key)
{
    SeriesKey temp_key;
    temp_key.m_series_key = series_key;
    return m_series_block.m_series_key_chunks.find(temp_key) == m_series_block.m_series_key_chunks.end();
}