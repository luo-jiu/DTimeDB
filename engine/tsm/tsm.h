#pragma once

#include <json/json.h>
using namespace luo::json;

#include <chrono>
using namespace std::chrono;

#include <engine/tsm/file_manager.h>

#include <iostream>
#include <fstream>
#include <list>
#include <iomanip>

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

        public:
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
                    Json temp(my_to_string(value));
                    values.append(temp);
                }
                json["value"] = values;
                return json;
            }

            string my_to_string(const T& value);
            void setLength(int32_t length) { m_length = length; }

            bool write(high_resolution_clock::time_point timestamp, T value);

        public:
            Type                                                m_type;
            int32_t                                             m_length;
            std::list<high_resolution_clock::time_point>        m_timestamps;
            std::list<T>                                        m_values;
        };

        class IndexBlock
        {
        public:
        };

        class IndexEntry
        {
        public:


        public:
            high_resolution_clock::time_point       m_max_time;
            high_resolution_clock::time_point       m_min_time;
            int64_t                                 m_offset;  // 数据块起始位置
            int32_t                                 m_size;  // 数据块大小
        };

        class TSM
        {
        public:
            bool write_header_to_file(const Header & header, const string & filename);
            bool read_header_from_file(Header & header, const string & filename);

            // 写入读取data block integer & float
            template <class T>
            bool write_data_to_file(const DataBlock<T> & dataBlock ,const string & filename) const;

            template <class T>
            bool read_data_from_file(DataBlock<T> & dataBlock, const string & filename);

            // 写入读取data block string
            template <class T>
            bool write_data_string_to_file(const DataBlock<T> & dataBlock ,const string & filename) const;

            template <class T>
            bool read_data_string_from_file(DataBlock<T> & dataBlock, const string & filename);
        };

        template <class T>
        bool TSM::write_data_to_file(const DataBlock<T> & dataBlock ,const string & filename) const
        {
            auto & file = FileManager::get_output_stream(filename);
            if (!file.is_open())
            {
                std::cerr << "Error: Could not open file for writing - from engine/tsm/tsm.h" << std::endl;
                return false;
            }

            // 将类型 长度写入文件
            file.write(reinterpret_cast<const char*>(&dataBlock.m_type), sizeof(dataBlock.m_type));
            file.write(reinterpret_cast<const char*>(&dataBlock.m_length), sizeof(dataBlock.m_length));
            // 将时间戳写入文件
            for (const auto & timestamp : dataBlock.m_timestamps)
            {
                file.write(reinterpret_cast<const char*>(&timestamp), sizeof(timestamp));
            }
            // 将数据写入文件
            for (const auto & value : dataBlock.m_values)
            {
                file.write(reinterpret_cast<const char*>(&value), sizeof(value));
            }
            file.flush();
            return true;
        }

        template <class T>
        bool TSM::read_data_from_file(DataBlock<T> & dataBlock, const string & filename)
        {
            auto & file = FileManager::get_input_stream(filename);
            if (!file.is_open())
            {
                std::cerr << "Error: Could not open file for reading - from engine/tsm/tsm.h" << std::endl;
                return false;
            }

            // 读取类型 长度
            file.read(reinterpret_cast<char*>(&dataBlock.m_type), sizeof(dataBlock.m_type));
            file.read(reinterpret_cast<char*>(&dataBlock.m_length), sizeof(dataBlock.m_length));
            // 读取时间戳
            dataBlock.m_timestamps.clear();
            auto num = dataBlock.m_length;
            for (size_t i = 0; i < num; ++i)
            {
                high_resolution_clock::time_point timestamp;
                file.read(reinterpret_cast<char*>(&timestamp), sizeof(timestamp));
                dataBlock.m_timestamps.push_back(timestamp);
            }
            // 读取值
            dataBlock.m_values.clear();
            for (size_t i = 0; i < num; ++i)
            {
                T value;
                file.read(reinterpret_cast<char*>(&value), sizeof(value));
                dataBlock.m_values.push_back(value);
            }
            return true;
        }

        template <class T>
        bool TSM::write_data_string_to_file(const DataBlock<T> & dataBlock ,const string & filename) const
        {
            auto & file = FileManager::get_output_stream(filename);
            if (!file.is_open())
            {
                std::cerr << "Error: Could not open file for writing - from engine/tsm/tsm.h" << std::endl;
                return false;
            }

            // 将类型 长度写入文件
            file.write(reinterpret_cast<const char*>(&dataBlock.m_type), sizeof(dataBlock.m_type));
            file.write(reinterpret_cast<const char*>(&dataBlock.m_length), sizeof(dataBlock.m_length));
            // 将时间戳写入文件
            for (const auto & timestamp : dataBlock.m_timestamps)
            {
                file.write(reinterpret_cast<const char*>(&timestamp), sizeof(timestamp));
            }
            // 将string 写入文件
            for (const string & value : dataBlock.m_values)
            {
                auto length = static_cast<int32_t>(value.length());
                file.write(reinterpret_cast<const char*>(&length), sizeof(int32_t));
                file.write(value.c_str(), length);
            }
            file.flush();
            return true;
        }

        template <class T>
        bool TSM::read_data_string_from_file(DataBlock<T> & dataBlock, const string & filename)
        {
            auto & file = FileManager::get_input_stream(filename);
            if (!file.is_open())
            {
                std::cerr << "Error: Could not open file for reading - from engine/tsm/tsm.h" << std::endl;
                return false;
            }

            // 读取类型 长度
            file.read(reinterpret_cast<char*>(&dataBlock.m_type), sizeof(dataBlock.m_type));
            file.read(reinterpret_cast<char*>(&dataBlock.m_length), sizeof(dataBlock.m_length));
            // 读取时间戳
            dataBlock.m_timestamps.clear();
            auto num = dataBlock.m_length;
            for (size_t i = 0; i < num; ++i)
            {
                high_resolution_clock::time_point timestamp;
                file.read(reinterpret_cast<char*>(&timestamp), sizeof(timestamp));
                dataBlock.m_timestamps.push_back(timestamp);
            }
            // 读取值
            for (size_t i = 0; i < num; ++i)
            {
                int32_t length;
                file.read(reinterpret_cast<char*>(&length), sizeof(int32_t));
                if (file.good())
                {
                    char buffer[length + 1];
                    file.read(buffer, length);
                    buffer[length] = '\0';
                    dataBlock.m_values.push_back(buffer);
                }
            }
            return true;
        }

        template <typename T>
        string DataBlock<T>::my_to_string(const T& value) {
            std::ostringstream oss;
            oss << value;
            return oss.str();
        }

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
    }
}








