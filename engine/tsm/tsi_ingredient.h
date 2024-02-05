#ifndef DTIMEDB_TSI_INGREDIENT_H
#define DTIMEDB_TSI_INGREDIENT_H

#include <vector>
#include <string>
using std::string;

namespace dt::tsm
{
    class BlockTrailer
    {
        uint8_t m_data_offset;
        uint32_t m_data_size;
    };

    /**
     * 存储了数据库中所有SeriesKey
     */
    class SeriesBlock
    {

    };

    class TagValue
    {

    };

    /**
     * 实际上是seriesByTagKeyValue这个双重
     * map–map<tag_key, map<tag_value, List<SeriesID>>>在文件中的实际存储
     */
    class TagBlock
    {

    };

    class Measurement
    {
        string          m_name;  // 测量值名
        uint8_t         m_tag_block_offset;
        uint32_t        m_tag_block_size;
        uint32_t        m_series_count;  // 与该Measurement相关联的时间序列数据的数量
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
