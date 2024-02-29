#include "engine/tsm/index.h"
#include <mutex>
using namespace dt::tsm;

using std::string;

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
        std::unique_lock<std::shared_mutex> write_lock(m_mutex);

        std::cout << "series_key不存在,准备添加到倒排索引\n";
        SeriesKey temp_key{series_key};
        m_series_block.m_series_key_chunks.insert(temp_key);

        // 双map 写入
        auto mea_it = m_mea_tag_keys.find(measurement);
        if (mea_it == m_mea_tag_keys.end()) {
            // 如果找不到 measurement，就创建一个新的 TagBlock 并插入
            TagBlock new_tag_block;
            mea_it = m_mea_tag_keys.insert({measurement, new_tag_block}).first;
        }

        auto& tag_block = mea_it->second;
        for (auto& tag : tags) {
            size_t pos = tag.find('=');
            if (pos != std::string::npos) {
                string key = tag.substr(0, pos);
                string value = tag.substr(pos + 1);
                // 直接插入 key 和 value 到嵌套的 unordered_map 和 set 中
                tag_block.m_series_by_tag_key_value[key][value].insert(series_key);

                // 单map 写入[tag_key]
                m_mea_tags[measurement].insert(key);
            }
        }
    }
}

/**
 * 判断目标值是不是一个标签
 */
bool Index::whether_tag(
        const std::string & measurement,
        std::string & target)
{
    std::shared_lock<std::shared_mutex> read_lock(m_mutex);
    auto tags_it = m_mea_tags.find(measurement);
    if (tags_it != m_mea_tags.end())
    {
        return tags_it->second.find(target) != tags_it->second.end();
    }
    return false;
}

/**
 * 判断seriesKey 是否存在
 */
bool Index::series_key_empty(
        string & series_key)
{
    std::shared_lock<std::shared_mutex> read_lock(m_mutex);
    SeriesKey temp_key;
    temp_key.m_series_key = series_key;
    return m_series_block.m_series_key_chunks.find(temp_key) == m_series_block.m_series_key_chunks.end();
}

std::set<std::string> Index::get_series_key_by_tag(
        const std::string & measurement,
        const std::pair<std::string, std::string>& tag)
{
    return m_mea_tag_keys[measurement].m_series_by_tag_key_value[tag.first][tag.second];
}