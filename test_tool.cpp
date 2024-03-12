#include "engine/tsm/tsm_ingredient.h"
#include "engine/tsm/iterator.h"
#include "file_manager/file_io_manager.h"
#include "snappy.h"
#include <iostream>
#include <chrono>
#include <cstring>

using namespace std::chrono;
using std::string;
/**
 * 解压snappy
 */
std::string decompress_data(
        const std::string & compressed_data)
{
    std::string decompressed_data;
    // 使用 Snappy 的 Uncompress 方法解压数据
    if (!snappy::Uncompress(compressed_data.data(), compressed_data.size(), &decompressed_data)) {
        std::cerr << "Failed to decompress data." << std::endl;
        // 处理解压失败的情况
    }
    return decompressed_data;
}

/**
 * 逆差值
 */
std::list<high_resolution_clock::time_point> restore_timestamps(
        const std::vector<nanoseconds> & differences) {
    std::list<high_resolution_clock::time_point> timestamps;

    if (!differences.empty())
    {
        // 从差值序列的第一个元素恢复第一个时间戳
        auto it = differences.begin();
        auto epoch = high_resolution_clock::time_point(*it);
        timestamps.push_back(epoch);

        // 之后的每个时间戳都是前一个时间戳加上差值
        auto previousTimestamp = epoch;
        ++it;
        for (; it != differences.end(); ++it)
        {
            previousTimestamp += *it;
            timestamps.push_back(previousTimestamp);
        }
    }
    return timestamps;
}


std::list<std::string> deserialize_strings(
        const string & serialized) {
    std::list<std::string> strings;
    size_t i = 0;
    while (i < serialized.size())
    {
        // 反序列化字符串长度
        if (i + sizeof(int) > serialized.size())
        {
            throw std::runtime_error("Serialized data is corrupted or incomplete.");
        }
        int length;
        std::memcpy(&length, serialized.data() + i, sizeof(int));
        i += sizeof(int);

        // 反序列化字符串内容
        if (i + length > serialized.size())
        {
            throw std::runtime_error("Serialized data is corrupted or incomplete.");
        }
        strings.emplace_back(serialized.substr(i, length));
        i += length;
    }
    return strings;
}

/**
 * 反序列snappy
 */
std::vector<nanoseconds> deserialize_differences(
        const std::string & serialized_data)
{
    std::vector<std::chrono::nanoseconds> differences;
    size_t i = 0;
    while (i < serialized_data.size())
    {
        long long count;
        // 确保在读取之前有足够的数据以避免越界
        if (i + sizeof(count) <= serialized_data.size())
        {
            std::memcpy(&count, serialized_data.data() + i, sizeof(count));
            differences.push_back(std::chrono::nanoseconds(count));
            i += sizeof(count);
        }
        else
        {
            // 处理错误或者不完整的序列化数据
            throw std::runtime_error("Serialized data is corrupted or incomplete.");
        }
    }
    return differences;
}

int main()
{
    dt::tsm::SeriesIndexBlock series_index_block;
    dt::file::FileIOManager io(3);

    auto file = io.get_file_stream("./../dbs/db_test/school-1710232254430683150.tsm" , "binary");
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for writing - m_from engine/tsm/tsm_.h" << std::endl;
        return 0;
    }

    // 读取index entry 偏移量
    file->seekg(-8, std::ios::end);  // 跳转到文件末尾

    int64_t tail = file->tellg();
    int64_t offset = 0;
    file->read(reinterpret_cast<char*>(&offset), sizeof(int64_t));
    file->seekg(offset);

    int64_t curr_pos = file->tellg();
    // 开始读取meta 和index_entry
    while (curr_pos < tail)
    {
        dt::tsm::IndexBlockMeta index_meta;
        uint16_t length;
        file->read(reinterpret_cast<char*>(&length), sizeof(uint16_t));  // series key

        char buffer[length + 1];
        file->read(buffer, length);
        buffer[length] = '\0';
        std::string series_key_and_field_name(buffer);
        index_meta.m_key_size = length;
        index_meta.m_key = series_key_and_field_name;
        file->read(reinterpret_cast<char*>(&index_meta.m_type), sizeof(index_meta.m_type));
        file->read(reinterpret_cast<char*>(&index_meta.m_count), sizeof(index_meta.m_count));
        series_index_block.m_meta = index_meta;

        // 清空数据
        series_index_block.m_index_entries.clear();

        // 开始读取index entry
        for(int i = 0; i < index_meta.m_count; ++i)
        {
            dt::tsm::IndexEntry entry;
            std::chrono::system_clock::time_point max;
            std::chrono::system_clock::time_point min;
            file->read(reinterpret_cast<char*>(&max), sizeof(std::chrono::system_clock::time_point));
            file->read(reinterpret_cast<char*>(&min), sizeof(std::chrono::system_clock::time_point));

            entry.set_max_time(max);
            entry.set_min_time(min);

            int64_t get_offset;
            file->read(reinterpret_cast<char*>(&get_offset), sizeof(get_offset));
            entry.set_offset(get_offset);

            int32_t size;
            file->read(reinterpret_cast<char*>(&size), sizeof(size));
            entry.set_size(size);
            series_index_block.m_index_entries.push_back(entry);
        }
        curr_pos = file->tellg();
        // 循环读取数据
        for (auto& entry : series_index_block.m_index_entries)
        {
            dt::tsm::DataBlock data_block;
            auto offset_temp = entry.get_offset();
            file->seekg(offset_temp);

            file->read(reinterpret_cast<char*>(&data_block.m_type), sizeof(data_block.m_type));
            file->read(reinterpret_cast<char*>(&data_block.m_num), sizeof(data_block.m_num));
            int32_t timestamp_size, value_size;
            file->read(reinterpret_cast<char*>(&timestamp_size), sizeof(timestamp_size));
            file->read(reinterpret_cast<char*>(&value_size), sizeof(value_size));
            std::cout << "curr_offset:" << file->tellg() << std::endl;

            data_block.m_tp_snappy_size = timestamp_size;
            data_block.m_val_snappy_size = value_size;

            // 读取并解压时间戳数据
            string compressed_timestamp(data_block.m_tp_snappy_size, '\0');
            file->read(&compressed_timestamp[0], data_block.m_tp_snappy_size);
            string timestamp_serialize;
            snappy::Uncompress(compressed_timestamp.data(), data_block.m_tp_snappy_size, &timestamp_serialize);
            auto timestamps =  deserialize_differences(timestamp_serialize);
            data_block.m_timestamps = restore_timestamps(timestamps);
            std::cout << "curr_offset:" << file->tellg() << std::endl;

            // 读取并解压值数据
            string compressed_val(data_block.m_val_snappy_size, '\0');
            file->read(&compressed_val[0], data_block.m_val_snappy_size);
            string val_serialize;
            snappy::Uncompress(compressed_val.data(), data_block.m_val_snappy_size, &val_serialize);
            data_block.m_values = deserialize_strings(val_serialize);
            std::cout << "curr_offset:" << file->tellg() << std::endl;

            std::cout << data_block.json() << std::endl;
        }
        file->seekg(curr_pos);
    }
}