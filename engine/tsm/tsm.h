#pragma once

#include "tsm_ingredient.h"

using std::string;

namespace dt
{
    namespace tsm
    {

        class TSM
        {
        public:
            // 写入读取header
            bool write_header_to_file(const Header & header, const string & file_path);
            bool read_header_from_file(Header & header, const string & file_path);

            // 写入读取data block integer & float
            template <class T>
            u_int64_t write_data_to_file(const DataBlock<T> & data_block ,const string & file_path) const;

            template <class T>
            bool read_data_from_file(DataBlock<T> & data_block, const string & file_path);

            // 写入读取data block string
            template <class T>
            u_int64_t write_data_string_to_file(const DataBlock<T> & data_block , const string & file_path) const;

            template <class T>
            bool read_data_string_from_file(DataBlock<T> & data_block, const string & file_path);

            // 写入读取footer
            bool write_footer_to_file(const Footer & footer, const string & file_path);
            bool read_footer_from_file(Footer & footer, const string & file_file);

            // 生成IndexEntry
            IndexEntry & create_index_entry(high_resolution_clock::time_point max_time, high_resolution_clock::time_point min_time, int64_t offset, int32_t size);
        };

        /**
         * 将数据块写入
         * @return 返回块大小
         */
        template <class T>
        u_int64_t TSM::write_data_to_file(const DataBlock<T> & data_block ,const string & file_path) const
        {
            auto & file = FileManager::get_output_stream(file_path);
            if (!file.is_open())
            {
                std::cerr << "Error: Could not open file for writing - from engine/tsm/tsm.h" << std::endl;
                return -1;
            }

            u_int64_t size = 8;
            long timestamp_size = sizeof(high_resolution_clock::time_point);

            // 将类型 长度写入文件
            file.write(reinterpret_cast<const char*>(&data_block.m_type), sizeof(data_block.m_type));
            file.write(reinterpret_cast<const char*>(&data_block.m_length), sizeof(data_block.m_length));

            // 将时间戳写入文件
            for (const auto & timestamp : data_block.m_timestamps)
            {
                file.write(reinterpret_cast<const char*>(&timestamp), sizeof(timestamp));
            }
            size += timestamp_size * data_block.m_timestamps.size();

            // 将数据写入文件
            for (const auto & value : data_block.m_values)
            {
                file.write(reinterpret_cast<const char*>(&value), sizeof(value));
            }
            size += 4 * data_block.m_values.size();

            file.flush();
            return size;
        }

        template <class T>
        bool TSM::read_data_from_file(DataBlock<T> & data_block, const string & file_path)
        {
            auto & file = FileManager::get_input_stream(file_path);
            if (!file.is_open())
            {
                std::cerr << "Error: Could not open file for reading - from engine/tsm/tsm.h" << std::endl;
                return false;
            }

            // 读取类型 长度
            file.read(reinterpret_cast<char*>(&data_block.m_type), sizeof(data_block.m_type));
            file.read(reinterpret_cast<char*>(&data_block.m_length), sizeof(data_block.m_length));

            // 读取时间戳
            data_block.m_timestamps.clear();
            auto num = data_block.m_length;
            for (size_t i = 0; i < num; ++i)
            {
                high_resolution_clock::time_point timestamp;
                file.read(reinterpret_cast<char*>(&timestamp), sizeof(timestamp));
                data_block.m_timestamps.push_back(timestamp);
            }

            // 读取值
            data_block.m_values.clear();
            for (size_t i = 0; i < num; ++i)
            {
                T value;
                file.read(reinterpret_cast<char*>(&value), sizeof(value));
                data_block.m_values.push_back(value);
            }

            return true;
        }

        template <class T>
        u_int64_t TSM::write_data_string_to_file(const DataBlock<T> & data_block , const string & file_path) const
        {
            auto & file = FileManager::get_output_stream(file_path);
            if (!file.is_open())
            {
                std::cerr << "Error: Could not open file for writing - from engine/tsm/tsm.h" << std::endl;
                return -1;
            }

            u_int64_t size = 8;

            // 将类型 长度写入文件
            file.write(reinterpret_cast<const char*>(&data_block.m_type), sizeof(data_block.m_type));
            file.write(reinterpret_cast<const char*>(&data_block.m_length), sizeof(data_block.m_length));

            long timestamp_size = sizeof(high_resolution_clock::time_point);
            long value_size = sizeof(u_int16_t);

            // 将时间戳写入文件
            for (const auto & timestamp : data_block.m_timestamps)
            {
                file.write(reinterpret_cast<const char*>(&timestamp), timestamp_size);
            }
            size += timestamp_size * data_block.m_timestamps.size();

            // 将string 写入文件
            for (const string & value : data_block.m_values)
            {
                auto length = static_cast<u_int16_t>(value.length());
                file.write(reinterpret_cast<const char*>(&length), sizeof(u_int16_t));  // 写大小
                file.write(value.c_str(), length);  // 写字符串
                size += (length + value_size);
            }

            file.flush();
            return size;
        }

        template <class T>
        bool TSM::read_data_string_from_file(DataBlock<T> & data_block, const string & file_path)
        {
            auto & file = FileManager::get_input_stream(file_path);
            if (!file.is_open())
            {
                std::cerr << "Error: Could not open file for reading - from engine/tsm/tsm.h" << std::endl;
                return false;
            }

            // 读取类型 长度
            file.read(reinterpret_cast<char*>(&data_block.m_type), sizeof(data_block.m_type));
            file.read(reinterpret_cast<char*>(&data_block.m_length), sizeof(data_block.m_length));

            // 读取时间戳
            data_block.m_timestamps.clear();
            auto num = data_block.m_length;
            for (size_t i = 0; i < num; ++i)
            {
                high_resolution_clock::time_point timestamp;
                file.read(reinterpret_cast<char*>(&timestamp), sizeof(timestamp));
                data_block.m_timestamps.push_back(timestamp);
            }

            // 读取值
            data_block.m_values.clear();
            for (size_t i = 0; i < num; ++i)
            {
                u_int16_t length;
                file.read(reinterpret_cast<char*>(&length), sizeof(u_int16_t));
                if (file.good())
                {
                    char buffer[length + 1];
                    file.read(buffer, length);
                    buffer[length] = '\0';
                    data_block.m_values.push_back(buffer);
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








