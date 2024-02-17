#ifndef DTIMEDB_TSI_INGREDIENT_H
#define DTIMEDB_TSI_INGREDIENT_H

#include <vector>
#include <map>
#include <list>
#include <string>

namespace dt::tsm
{
    class BlockTrailer
    {
        uint8_t m_data_offset;
        uint32_t m_data_size;
    };

    class Tag
    {
        std::string tag_key;
        std::string tag_value;
    };

    class SeriesKey
    {
    public:
        bool operator<(const SeriesKey & rhs) const
        {
            return m_series_key < rhs.m_series_key;
        }

    public:
        std::string m_series_key;
        std::vector<Tag> m_tags;
    };

    /**
     * 一个测量值分一个块
     */
    class SeriesKeyChunk
    {
        std::vector<SeriesKey> m_series;
    };

    /**
     * 存储了数据库中所有Series_key
     *
     * [作用]
     * 用于判断series_key 是否存在，存储了所有的series_key
     */
    class SeriesBlock
    {

    };

    class TagValueInfo
    {
    public:
        uint8_t m_value_length;
        std::string m_value;  // tag value
        uint16_t m_series_num;
        uint16_t m_series_data_length;    // series length
        std::list<std::string> m_series_data;  // series
    };



    class TagKeyInfo
    {

    };

    class TagValue
    {
    public:
        std::vector<TagValueInfo> m_tag_values;

    };

    /**
     * 实际上是seriesByTagKeyValue这个双重
     * map<tag_key, map<tag_value, List<SeriesID>>>在文件中的实际存储
     */
    class TagBlock
    {
    public:
        // map<tag_key, tag_value>
//        std::unordered_map<string, TagValue> m_tag_key_value;
    };

    /**
     * TagBlock 和 Measurement 一一对应
     */
    class Measurement
    {
        std::string          m_name;         // 测量值名
        uint8_t         m_tag_block_offset;  // 记录tag块偏移量
        uint32_t        m_tag_block_size;    // 记录块大小
        uint32_t        m_series_count;      // 与该Measurement相关联的时间序列数据的数量
        uint32_t        m_size;
    };

    class HashIndex
    {

    };

    /**
     * 存储数据库中表的信息，通常来说Measurement不会太多，一个Block也就够了
     */
    class MeasurementBlock
    {
        std::vector<Measurement> m_measurement;
    };

    /**
     * Measurement Block、Tag Block以及Series Block在TSI文件中的偏移量以及数据大小
     */
    class IndexFileTrailer
    {

    };

    class TSI
    {
        std::vector<TagBlock> m_tag_blocks;
    };
}

#endif //DTIMEDB_TSI_INGREDIENT_H
