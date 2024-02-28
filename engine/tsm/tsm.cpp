#include "engine/tsm/tsm.h"

#include <cstring>

using namespace dt::tsm;
using namespace std::chrono;

using std::string;

/**
 * 将TSM Header 写入到文件
 */
bool TSM::write_header_to_file(
        const Header & header,
        const string & file_path)
{
    auto file = m_file_manager.get_file_stream(file_path, "binary");
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for writing - m_from engine/tsm/write.cpp" << std::endl;
        return false;
    }
    file->write(reinterpret_cast<const char*>(&header), sizeof(header));

    file->flush();
    m_file_manager.release_file_stream(file_path);
    return true;
}

/**
 * 将TSM Header 读取到内存
 */
bool TSM::read_header_from_file(
        Header & header,
        const string & file_path)
{
    auto file = m_file_manager.get_file_stream(file_path, "binary");
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for reading - m_from engine/tsm/write.cpp" << std::endl;
        return false;
    }
    file->seekg(0);  // 移动至开头
    file->read(reinterpret_cast<char*>(&header), sizeof(header));
    m_file_manager.release_file_stream(file_path);
    return true;
}

int64_t TSM::write_data_to_file(
        DataBlock::Type type,
        int32_t data_num,
        const string & compress_timestamp,
        const string & compress_val,
        const string & file_path,
        int64_t offset)
{
    auto file = m_file_manager.get_file_stream(file_path, "binary");
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for writing - m_from engine/tsm/tsm_.h" << std::endl;
        return -1;
    }
    file->seekp(offset);  // 移动到指定位置

    int64_t timestamps_size = compress_timestamp.size();
    int64_t values_size = compress_val.size();

    // 将类型 长度写入文件
    file->write(reinterpret_cast<const char*>(&type), sizeof(type));
    file->write(reinterpret_cast<const char*>(&data_num), sizeof(int32_t));
    file->write(reinterpret_cast<const char*>(&timestamps_size), sizeof(int32_t));
    file->write(reinterpret_cast<const char*>(&values_size), sizeof(int32_t));

    // 将压缩后的timestamp 写入文件
    file->write(compress_timestamp.data(), timestamps_size);
    // 将压缩后的val 写入文件
    file->write(compress_val.data(), values_size);

    file->flush();
    m_file_manager.release_file_stream(file_path);

    // 测试读取
    auto data = std::make_shared<DataBlock>();
    read_data_from_file(data, file_path, 5);
    std::cout << data->json() << std::endl;
    int64_t block_size = 16 + timestamps_size + values_size;
    return block_size;
}

bool TSM::read_data_from_file(
        std::shared_ptr<DataBlock> & data_block,
        const string & file_path,
        int64_t offset)
{
    auto file = m_file_manager.get_file_stream(file_path, "binary");
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for reading - m_from engine/tsm/tsm_.h" << std::endl;
        return false;
    }
    file->seekg(offset);

    // 读取类型 长度
    file->read(reinterpret_cast<char*>(&data_block->m_type), sizeof(data_block->m_type));
    file->read(reinterpret_cast<char*>(&data_block->m_num), sizeof(data_block->m_num));
    file->read(reinterpret_cast<char*>(&data_block->m_tp_snappy_size), sizeof(data_block->m_tp_snappy_size));
    file->read(reinterpret_cast<char*>(&data_block->m_val_snappy_size), sizeof(data_block->m_val_snappy_size));

    // 读取并解压时间戳数据
    string compressed_timestamp(data_block->m_tp_snappy_size, '\0');
    file->read(&compressed_timestamp[0], data_block->m_tp_snappy_size);
    string timestamp_serialize;
    snappy::Uncompress(compressed_timestamp.data(), data_block->m_tp_snappy_size, &timestamp_serialize);
    auto timestamps =  deserialize_differences(timestamp_serialize);
    data_block->m_timestamps = restore_timestamps(timestamps);

    // 读取并解压值数据
    string compressed_val(data_block->m_val_snappy_size, '\0');
    file->read(&compressed_val[0], data_block->m_val_snappy_size);
    string val_serialize;
    snappy::Uncompress(compressed_val.data(), data_block->m_val_snappy_size, &val_serialize);
    data_block->m_values = deserialize_strings(val_serialize);

    m_file_manager.release_file_stream(file_path);
    return true;
}

int64_t TSM::write_index_entry_to_file(
        const std::shared_ptr<IndexEntry> & index_entry,
        const string & file_path,
        int64_t offset)
{
    auto file = m_file_manager.get_file_stream(file_path, "binary");
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for writing - m_from engine/tsm/tsm_.h" << std::endl;
        return false;
    }

    file->seekp(offset);

    long timestamp_size = sizeof(high_resolution_clock::time_point);
    file->write(reinterpret_cast<const char*>(&index_entry->get_max_time()), timestamp_size);
    file->write(reinterpret_cast<const char*>(&index_entry->get_min_time()), timestamp_size);

    int64_t get_offset = index_entry->get_offset();
    file->write(reinterpret_cast<const char*>(&get_offset), sizeof(int64_t));

    int64_t size = index_entry->get_size();
    file->write(reinterpret_cast<const char*>(&size), sizeof(int32_t));

    file->flush();
    m_file_manager.release_file_stream(file_path);
    return true;
}

bool TSM::read_index_entry_from_file(
        std::shared_ptr<IndexEntry> & index_entry,
        const string & file_path,
        int64_t offset)
{
    auto file = m_file_manager.get_file_stream(file_path, "binary");
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for reading - m_from engine/tsm/tsm_.h" << std::endl;
        return false;
    }
    file->seekg(offset);
    m_file_manager.release_file_stream(file_path);
}

int64_t TSM::write_index_meta_to_file(
        const std::shared_ptr<IndexBlockMeta> & index_meta,
        const string & file_path,
        int64_t offset)
{
    auto file = m_file_manager.get_file_stream(file_path, "binary");
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for writing - m_from engine/tsm/tsm_.h" << std::endl;
        return false;
    }
    file->seekp(offset);

    int64_t size = 12;
    uint16_t length = index_meta->get_key_length();
    file->write(reinterpret_cast<const char*>(&length), sizeof(uint16_t));
    file->write(index_meta->get_key().c_str(), length);  // 写字符串
    auto type = index_meta->get_type();
    file->write(reinterpret_cast<const char*>(&type), sizeof(index_meta->get_type()));
    uint16_t count = index_meta->get_count();
    file->write(reinterpret_cast<const char*>(&count), sizeof(uint16_t));

    file->flush();
    m_file_manager.release_file_stream(file_path);
    return size + length;
}

bool TSM::read_index_meta_from_file(
        std::shared_ptr<IndexBlockMeta> & index_meta,
        const string & file_path,
        int64_t offset)
{
    auto file = m_file_manager.get_file_stream(file_path, "binary");
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for reading - m_from engine/tsm/tsm_.h" << std::endl;
        return false;
    }
    file->seekg(offset);

    uint16_t key_size;
    file->read(reinterpret_cast<char*>(&key_size), sizeof(uint16_t));

    char buffer[key_size + 1];
    file->read(buffer, key_size);
    buffer[key_size] = '\0';

    DataBlock::Type type;
    file->read(reinterpret_cast<char*>(&type), sizeof(DataBlock::Type));

    uint16_t count;
    file->read(reinterpret_cast<char*>(&count), sizeof(uint16_t));

    index_meta->set_key_length(key_size);
    index_meta->set_key(buffer);
    index_meta->set_type(type);
    index_meta->set_count(count);

    m_file_manager.release_file_stream(file_path);
    return true;
}

/**
 * 将TSM Footer 写到末尾
 */
bool TSM::write_footer_to_file(
        const Footer & footer,
        const string & file_path,
        int64_t tail_offset)
{
    auto file = m_file_manager.get_file_stream(file_path, "binary");
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for reading - m_from engine/tsm/write.cpp" << std::endl;
        return false;
    }

    file->seekp(tail_offset);  // 跳转到文件末尾
    file->write(reinterpret_cast<const char*>(&footer), sizeof(footer));

    file->flush();
    m_file_manager.release_file_stream(file_path);
    return true;
}

/**
 * 将TSM Footer 读取出来
 */
bool TSM::read_footer_from_file(Footer & footer, const string & file_path)
{
    auto file = m_file_manager.get_file_stream(file_path, "binary");
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for reading - m_from engine/tsm/write.cpp" << std::endl;
        return false;
    }
    file->seekg(-8, std::ios::end);  // 跳转到Footer 开头
    file->read(reinterpret_cast<char*>(&footer), sizeof(footer));

    m_file_manager.release_file_stream(file_path);
    return true;
}

bool TSM::write_series_index_block_to_file(
        std::deque<std::shared_ptr<IndexEntry>> & index_entry,
        std::shared_ptr<IndexBlockMeta> & meta,
        const string & file_path,
        int64_t tail_offset)
{
    if (index_entry.empty() || meta == nullptr)
    {
        return false;
    }
    // 先写入meta
    auto meta_size = write_index_meta_to_file(meta, file_path, tail_offset);
    tail_offset += meta_size;

    // 再写入entry
    for (auto & entry : index_entry)
    {
        write_index_entry_to_file(entry, file_path, tail_offset);
        tail_offset += 28;
    }
    return true;
}

/**
 * 创建IndexEntry
 */
std::shared_ptr<IndexEntry> TSM::create_index_entry(
        high_resolution_clock::time_point max_time,
        high_resolution_clock::time_point min_time,
        int64_t offset,
        int32_t size)
{
    std::shared_ptr<IndexEntry> e(new IndexEntry(max_time, min_time, offset, size));
    return e;
}

/**
 * 创建IndexBlockMeta
 */
std::shared_ptr<IndexBlockMeta> TSM::create_index_meta(
        DataBlock::Type type,
        const string & meta_key)
{
    auto length = static_cast<uint16_t>(meta_key.length());

    switch (type) {
        case DataBlock::DATA_INTEGER:
        {
            std::shared_ptr<IndexBlockMeta> e(new IndexBlockMeta(length, meta_key, DataBlock::DATA_INTEGER));
            return e;
        }
        case DataBlock::DATA_STRING:
        {
            std::shared_ptr<IndexBlockMeta> e(new IndexBlockMeta(length, meta_key, DataBlock::DATA_STRING));
            return e;
        }
        case DataBlock::DATA_FLOAT:
        {
            std::shared_ptr<IndexBlockMeta> e(new IndexBlockMeta(length, meta_key, DataBlock::DATA_FLOAT));
            return e;
        }
        default:
        {
            std::cerr << "Error : Type '" << type << "' does not meet - m_from engine/tsm_/tsm_.cpp" << std::endl;
            return nullptr;
        }
    }
}

/**
 * 创建TSM File
 */
bool TSM::create_tsm(
        const Header & header,
        const Footer & footer,
        const string & file_path)
{
    auto file = m_file_manager.get_file_stream(file_path, "binary");
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for reading - m_from engine/tsm_/write.cpp" << std::endl;
        return false;
    }
    file->seekg(0);  // 文件指针移到开头
    file->write(reinterpret_cast<const char*>(&header), sizeof(header));  // 写入header
    file->seekp(4 * 1024 * 1024);
    file->write(reinterpret_cast<const char*>(&footer), sizeof(footer));  // 写入footer
    file->flush();

    m_file_manager.release_file_stream(file_path);
    return true;
}

/**
 * 对时间戳进行差值计算
 */
std::vector<nanoseconds> TSM::calculate_differences(
        const std::list<high_resolution_clock::time_point> & timestamps)
{
    std::vector<nanoseconds> differences;
    auto it = timestamps.begin();
    auto end = timestamps.end();

    // 确保至少有一个时间点
    if (it != end) {
        // 保存第一个时间点，转换为自纪元以来的纳秒数
        auto epoch = *it;
        auto firstTimestamp = duration_cast<nanoseconds>(epoch.time_since_epoch());
        differences.push_back(firstTimestamp);

        // 设置previous为第一个时间点
        auto previous = *it;
        ++it;

        // 遍历并计算与前一个时间点的差异
        for (; it != end; ++it) {
            auto diff = duration_cast<nanoseconds>(*it - previous);
            differences.push_back(diff);
            previous = *it;
        }
    }

    return differences;
}

/**
 * 恢复差值的时间戳
 * @param differences
 * @return
 */
std::list<high_resolution_clock::time_point> TSM::restore_timestamps(
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

/**
 * 序列化timestamp
 */
string TSM::serialize_differences(
        const std::vector<nanoseconds> & differences)
{
    string serializedData;
    for (const auto& diff : differences)
    {
        auto count = diff.count();
        serializedData.append(reinterpret_cast<const char*>(&count), sizeof(count));
    }
    return serializedData;
}

std::vector<nanoseconds> TSM::deserialize_differences(
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

string TSM::serialize_strings(
        const std::list<std::string>& strings)
{
    std::string serialized;
    for (const auto& str : strings)
    {
        // 序列化字符串长度
        int length = str.size();
        serialized.append(reinterpret_cast<const char*>(&length), sizeof(length));
        // 序列化字符串内容
        serialized.append(str);
    }
    return serialized;
}

std::list<std::string> TSM::deserialize_strings(
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


string TSM::compress_data(
        const string & serialized_data)
{
    string compressed_data;
    snappy::Compress(serialized_data.data(), serialized_data.size(), &compressed_data);
    return compressed_data;  // 注意压缩完后是一个二进制格式存储在string 里面
}

string TSM::decompress_data(
        const string & compressed_data)
{
    string decompressed_data;
    // 使用 Snappy 的 Uncompress 方法解压数据
    if (!snappy::Uncompress(compressed_data.data(), compressed_data.size(), &decompressed_data)) {
        std::cerr << "Failed to decompress data." << std::endl;
        // 处理解压失败的情况
    }
    return decompressed_data;
}

/**
 * 计算时间戳需要对实际大小
 */
string TSM::calculate_timestamp_size(
        const std::list<high_resolution_clock::time_point> & timestamps)
{
    // 对时间戳进行差值计算
    auto timestamp_differences = calculate_differences(timestamps);
    // 序列化差值后的timestamp
    auto timestamp_serialize = serialize_differences(timestamp_differences);
    // 将差值计算后的时间戳使用snappy 压缩
    return compress_data(timestamp_serialize);
}

/**
 * 计算数据需要的实际大小
 */
string TSM::calculate_val_size(
        const std::list<string> & values)
{
    // 序列化val
    auto val_serialize = serialize_strings(values);
    return compress_data(val_serialize);
}

int64_t TSM::flush_shard_in_meta_file(
        const std::shared_ptr<std::fstream> & stream,
        std::shared_ptr<Shard> & shard,
        int64_t offset)
{
    std::cout << "flush_shard_in_meta_file\n";
    if (!stream->is_open())
    {
        std::cerr << "Error: Could not open file for writing - m_from engine/tsm/tsm_.h" << std::endl;
        return -1;
    }
    stream->seekp(offset);  // 移动到指定位置

    // 序列化
    string val_serialize;
    shard->SerializeToString(&val_serialize);

    uint16_t length = val_serialize.size();
    stream->write(reinterpret_cast<const char*>(&length), sizeof(uint16_t));
    stream->write(val_serialize.c_str(), length);  // 写字符串

    m_file_manager.release_file_stream("./../metadata/meta.tsm");

    // 当即反序列化尝试
    Shard test;
    test.ParseFromString(val_serialize);
    std::cout << "尝试反序列化";
    std::cout<< test.DebugString() << std::endl;

    return offset + static_cast<int64_t>(sizeof(length)) + length;
}