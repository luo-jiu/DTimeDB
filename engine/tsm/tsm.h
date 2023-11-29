#pragma once

#include <list>
#include <json/json.h>
using namespace luo::json;

#include <chrono>
using namespace std::chrono;

#include <iostream>
#include <fstream>

using std::string;

namespace dt
{
    namespace tsm
    {
        /**
         * 页头
         */
        class Header
        {
        public:
            Header() {}
            Header(int32_t magic, int8_t version): m_magic(magic), m_version(version) {}
            ~Header() = default;

        private:
            int32_t m_magic;
            int8_t m_version;
        };

        /**
         * 页脚
         */
        class Footer
        {
        public:
            Footer() {}
            ~Footer() = default;

            int64_t getOffset() { return m_offset; }
            void setOffset(int64_t offset) { m_offset = offset; }
        private:
            int64_t m_offset;
        };

        template <class T>
        class DataBlock
        {
        public:
            enum Type
            {
                DATA_INTEGER,
                DATA_STRING,
                DATA_FLOAT,
            };

            DataBlock() {}
            DataBlock(Type type): m_type(type) {}
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
                    Json temp(std::to_string(value));
                    values.append(temp);
                }
                json["value"] = values;

                return json;
            }

            std::list<high_resolution_clock::time_point> getTimestamp() { return m_timestamps; }
            std::list<T> getValue() { return m_values; }
            void setLength(int32_t length) { m_length = length; }

            bool write(high_resolution_clock::time_point timestamp, T value);
            bool write_to_file(const std::string & filename) const;
            bool read_from_file(const std::string & filename);

        private:
            Type                                                m_type;
            int32_t                                             m_length;
            std::list<high_resolution_clock::time_point>        m_timestamps;
            std::list<T>                                        m_values;
        };

        class IndexBlock
        {

        };

        class IndexEntry
        {
        public:

        private:
            high_resolution_clock::time_point       max_time;
            high_resolution_clock::time_point       min_time;
            int64_t                                 m_offset;  // 数据块起始位置
            int32_t                                 m_size;  // 数据块大小
        };

        /**
         * 将时间戳和值写入到容器中
         * @tparam T 值类型
         * @param timestamp 时间戳
         * @param value 值
         * @return
         */
        template <typename T>
        bool DataBlock<T>::write(
                high_resolution_clock::time_point timestamp,
                T value)
        {
            if (timestamp.time_since_epoch() == high_resolution_clock::duration::zero() || std::is_same<T, std::nullptr_t>::value)
            {
                return false;
            }
            m_timestamps.push_back(timestamp);
            m_values.push_back(value);
            return true;
        }

        template <class T>
        bool DataBlock<T>::write_to_file(
                const std::string & filename) const
        {
            std::ofstream file(filename, std::ios::binary);
            if (!file.is_open())
            {
                std::cerr << "Error: Could not open file for writing - from engine/tsm/tsm.h" << std::endl;
                return false;
            }

            // 将类型 长度写入文件
            file.write(reinterpret_cast<const char*>(&m_type), sizeof(m_type));
            file.write(reinterpret_cast<const char*>(&m_length), sizeof(m_length));
            // 将时间戳写入文件
            for (const auto & timestamp : m_timestamps)
            {
                file.write(reinterpret_cast<const char*>(&timestamp), sizeof(timestamp));
            }
            for (const auto & value : m_values)
            {
                file.write(reinterpret_cast<const char*>(&value), sizeof(value));
            }
            file.close();
            return true;
        }

        template <class T>
        bool DataBlock<T>::read_from_file(
                const std::string & filename)
        {
            std::ifstream file(filename, std::ios::binary);
            if (!file.is_open())
            {
                std::cerr << "Error: Could not open file for reading - from engine/tsm/tsm.h" << std::endl;
                return false;
            }

            // 读取类型 长度
            file.read(reinterpret_cast<char*>(&m_type), sizeof(m_type));
            file.read(reinterpret_cast<char*>(&m_length), sizeof(m_length));

            // 读取时间戳
            m_timestamps.clear();
            for (size_t i = 0; i < m_length; ++i)
            {
                high_resolution_clock::time_point timestamp;
                file.read(reinterpret_cast<char*>(&timestamp), sizeof(timestamp));
                m_timestamps.push_back(timestamp);
            }

            // 读取值
            m_values.clear();
            for (size_t i = 0; i < m_length; ++i)
            {
                T value;
                file.read(reinterpret_cast<char*>(&value), sizeof(value));
                m_values.push_back(value);
            }
            file.close();
            return true;
        }
    }
}








