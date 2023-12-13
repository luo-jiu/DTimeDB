#ifndef DTIMEDB_TSM_INGREDIENT_H
#define DTIMEDB_TSM_INGREDIENT_H

#include <json/json.h>
using namespace luo::json;

#include <chrono>
using namespace std::chrono;

#include <engine/tsm/file_manager.h>

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

        Json json()
        {
            Json json;
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
        Footer(int32_t offset): m_offset(offset) {}
        ~Footer() = default;

        Json json()
        {
            Json json;
            json["offset"] = std::to_string(m_offset);
            return json;
        }

        int64_t getOffset() { return m_offset; }
        void setOffset(int64_t offset) { m_offset = offset; }

    private:
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

        DataBlock(): m_size(0), m_length(0){}
        DataBlock(Type type): m_type(type) , m_length(0){}
        ~DataBlock() = default;

        Json json()
        {
            Json json;
            json["type"] = m_type;
            json["length"] = m_length;

            Json timestamps;
            for (const auto& timestamp : m_timestamps)
            {
                auto nano_time_point = time_point_cast<nanoseconds>(timestamp);  // 将时间点转换为纳秒进度的时间点
                auto nano_duration = nano_time_point.time_since_epoch();  // 将时间节点转换为纳秒
                auto nano_count = nano_duration.count();
                Json temp(std::to_string(nano_count));  // 将纳秒数转换为字符串
                timestamps.append(temp);
            }
            json["timestamps"] = timestamps;

            Json values;
            for (auto & value : m_values)
            {
                Json temp(my_to_string(value));
                values.append(temp);
            }
            json["value"] = values;
            return json;
        }

        string my_to_string(const string & value);
        bool write(high_resolution_clock::time_point timestamp, const string & value);

    public:
        int32_t                                             m_size;  // 大小(不存入磁盘)
        high_resolution_clock::time_point                   m_max_timestamp;
        high_resolution_clock::time_point                   m_min_timestamp;

        Type                                                m_type;
        int32_t                                             m_length;
        std::list<high_resolution_clock::time_point>        m_timestamps;
        std::list<string>                                   m_values;
    };

    /**
     * size: 28
     */
    class IndexEntry
    {
    public:
        IndexEntry() {}
        IndexEntry(const high_resolution_clock::time_point & mMaxTime, const high_resolution_clock::time_point & mMinTime, int64_t mOffset, int32_t mSize)
                : m_max_time(mMaxTime), m_min_time(mMinTime), m_offset(mOffset), m_size(mSize) {}

        ~IndexEntry() = default;

        // get and set
        const high_resolution_clock::time_point & get_max_time() const { return m_max_time;}
        void set_max_time(const high_resolution_clock::time_point & max_time) { m_max_time = max_time; }
        const high_resolution_clock::time_point & get_min_time() const { return m_min_time; }
        void set_min_time(const high_resolution_clock::time_point & min_time) { m_min_time = min_time; }
        int64_t get_offset() const { return m_offset; }
        void set_offset(int64_t offset) { m_offset = offset; }
        int32_t get_size() const { return m_size; }
        void set_size(int32_t size) { m_size = size; }

    private:
        high_resolution_clock::time_point       m_max_time;
        high_resolution_clock::time_point       m_min_time;
        int64_t                                 m_offset;  // 数据块起始位置
        int32_t                                 m_size;  // 数据块大小
    };

    /**
     * size: 12 + string
     */
    class IndexBlockMeta
    {
    public:
        enum Type
        {
            DATA_INTEGER,
            DATA_STRING,
            DATA_FLOAT,
        };

        IndexBlockMeta() {}
        IndexBlockMeta(u_int16_t key_size, string key, Type type): m_key_size(key_size), m_key(std::move(key)), m_type(type){}
        ~IndexBlockMeta() {}

        // get and set
        u_int16_t get_key_length() const { return m_key_size; }
        void set_key_length(u_int16_t key_length) {
            m_key_size = key_length;
        }
        const string & get_key() const { return m_key; }
        void set_key(const string & key) { m_key = key; }
        Type get_type() const { return m_type; }
        void set_type(Type type) { m_type = type; }
        u_int16_t get_count() const { return m_count; }
        void set_count(u_int16_t count) { m_count = count; }

    private:
        u_int16_t                   m_key_size;
        string                      m_key;  // seriesKey + fieldKey
        Type                        m_type;
        u_int16_t                   m_count;  // entry count
    };
}

#endif //DTIMEDB_TSM_INGREDIENT_H