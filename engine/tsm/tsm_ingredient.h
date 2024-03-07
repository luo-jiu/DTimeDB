#ifndef DTIMEDB_TSM_INGREDIENT_H
#define DTIMEDB_TSM_INGREDIENT_H

#include "json/json.h"

#include <chrono>
#include <memory>
#include <iostream>
#include <fstream>
#include <list>
#include <iomanip>
#include <utility>

namespace dt::tsm
{
    /**
     * 页头
     * size: 5
     */
    class Header
    {
    public:
        Header() {}
        Header(int32_t magic, int8_t version): m_magic(magic), m_version(version) {}
        ~Header() = default;

        dt::json::Json json()
        {
            dt::json::Json json;
            json["magic"] = std::to_string(m_magic);
            json["version"] = std::to_string(m_version);
            return json;
        }

    private:
        int32_t m_magic;
        int8_t m_version;
    };

    /**
     * 页脚
     * size: 8
     */
    class Footer
    {
    public:
        Footer() {}
        Footer(int64_t offset): m_offset(offset) {}
        ~Footer() = default;

        dt::json::Json json()
        {
            dt::json::Json json;
            json["offset"] = std::to_string(m_offset);
            return json;
        }

        int64_t getOffset() { return m_offset; }
        void setOffset(int64_t offset) { m_offset = offset; }

    public:
        int64_t m_offset;
    };

    class DataBlock
    {
    public:
        enum Type
        {
            DATA_INTEGER,
            DATA_STRING,
            DATA_FLOAT,
        };

        DataBlock(): m_size(0), m_num(0){}
        DataBlock(Type type): m_type(type) , m_num(0){}
        ~DataBlock() = default;

        dt::json::Json json()
        {
            dt::json::Json json;
            json["type"] = m_type;
            json["tp_len"] = m_tp_snappy_size;
            json["val_len"] = m_val_snappy_size;

            dt::json::Json timestamps;
            for (const auto& timestamp : m_timestamps)
            {
                auto nano_time_point = std::chrono::time_point_cast<std::chrono::nanoseconds>(timestamp);  // 将时间点转换为纳秒进度的时间点
                auto nano_duration = nano_time_point.time_since_epoch();  // 将时间节点转换为纳秒
                auto nano_count = nano_duration.count();
                dt::json::Json temp(std::to_string(nano_count));  // 将纳秒数转换为字符串
                timestamps.append(temp);
            }
            json["timestamps"] = timestamps;

            dt::json::Json values;
            for (auto & value : m_values)
            {
                dt::json::Json temp(my_to_string(value));
                values.append(temp);
            }
            json["value"] = values;
            return json;
        }

        std::string my_to_string(const std::string & value);
        bool write(std::chrono::high_resolution_clock::time_point timestamp, const std::string & value);

    public:
        int32_t                                                          m_size;  // 大小(不存入磁盘)
        std::string                                                      m_field_name;  // 字段名(不存)
        std::chrono::high_resolution_clock::time_point                   m_max_timestamp;
        std::chrono::high_resolution_clock::time_point                   m_min_timestamp;
        std::string                                                      m_shard_id;

        Type                                                             m_type;
        int32_t                                                          m_num;
        int32_t                                                          m_tp_snappy_size;
        int32_t                                                          m_val_snappy_size;
        std::list<std::chrono::high_resolution_clock::time_point>        m_timestamps;
        std::list<std::string>                                           m_values;
    };

    /**
     * size: 28
     */
    class IndexEntry
    {
    public:
        IndexEntry() {}
        IndexEntry(const std::chrono::high_resolution_clock::time_point & mMaxTime, const std::chrono::high_resolution_clock::time_point & mMinTime, int64_t mOffset, int32_t mSize)
                : m_max_time(mMaxTime), m_min_time(mMinTime), m_offset(mOffset), m_size(mSize) {}

        ~IndexEntry() = default;

        // get and set
        const std::chrono::high_resolution_clock::time_point & get_max_time() const { return m_max_time;}
        void set_max_time(const std::chrono::high_resolution_clock::time_point & max_time) { m_max_time = max_time; }
        const std::chrono::high_resolution_clock::time_point & get_min_time() const { return m_min_time; }
        void set_min_time(const std::chrono::high_resolution_clock::time_point & min_time) { m_min_time = min_time; }
        int64_t get_offset() const { return m_offset; }
        void set_offset(int64_t offset) { m_offset = offset; }
        int32_t get_size() const { return m_size; }
        void set_size(int32_t size) { m_size = size; }

    private:
        std::chrono::high_resolution_clock::time_point       m_max_time;
        std::chrono::high_resolution_clock::time_point       m_min_time;
        int64_t                                              m_offset;  // 数据块起始位置
        int32_t                                              m_size;  // 数据块大小
    };

    /**
     * size: 8 + string
     */
    class IndexBlockMeta
    {
    public:
        IndexBlockMeta() {}
        IndexBlockMeta(uint16_t key_size, std::string key, DataBlock::Type type): m_key_size(key_size), m_key(std::move(key)), m_type(type){}
        ~IndexBlockMeta() {}

        // get and set
        uint16_t get_key_length() const { return m_key_size; }
        void set_key_length(uint16_t key_length) {
            m_key_size = key_length;
        }
        const std::string & get_key() const { return m_key; }
        void set_key(const std::string & key) { m_key = key; }
        DataBlock::Type get_type() const { return m_type; }
        void set_type(DataBlock::Type type) { m_type = type; }
        uint16_t get_count() const { return m_count; }
        void set_count(uint16_t count) { m_count = count; }

    public:
        uint16_t                    m_key_size;
        std::string                 m_key;  // seriesKey + fieldKey
        DataBlock::Type             m_type;
        uint16_t                    m_count;  // entry count
    };
}

#endif //DTIMEDB_TSM_INGREDIENT_H