#include <engine/tsm/tsm.h>
#include <cstring>

using namespace dt::tsm;

/**
 * 将TSM Header 写入到文件
 */
bool TSM::write_header_to_file(
        const Header & header,
        const string & file_path)
{
    auto file = m_file_manager.get_file_stream(file_path);
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for writing - m_from engine/tsm_/write.cpp" << std::endl;
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
    auto file = m_file_manager.get_file_stream(file_path);
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for reading - m_from engine/tsm_/write.cpp" << std::endl;
        return false;
    }
    file->seekg(0);  // 移动至开头
    file->read(reinterpret_cast<char*>(&header), sizeof(header));
    m_file_manager.release_file_stream(file_path);
    return true;
}

uint64_t TSM::write_data_to_file(
        const std::shared_ptr<DataBlock> & data_block ,
        const string & file_path,
        int64_t offset)
{
    auto file = m_file_manager.get_file_stream(file_path);
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for writing - m_from engine/tsm_/tsm_.h" << std::endl;
        return -1;
    }
    file->seekp(offset);  // 移动到指定位置
    int32_t timestamps_size, values_size;

    // 对时间戳进行差值计算
    auto timestamp_differences = calculate_differences(data_block->m_timestamps);

    // 序列化差值后的timestamp
    auto timestamp_serialize = serialize_differences(timestamp_differences);
    // 序列化val
    auto val_serialize = serialize_strings(data_block->m_values);

    // 将差值计算后的时间戳使用snappy 压缩
    auto compress_timestamp = compress_data(timestamp_serialize);
    timestamps_size = compress_timestamp.size();
    auto compress_val = compress_data(val_serialize);
    values_size = compress_val.size();

    // 将类型 长度写入文件
    file->write(reinterpret_cast<const char*>(&data_block->m_type), sizeof(data_block->m_type));
    file->write(reinterpret_cast<const char*>(&timestamps_size), sizeof(int32_t));
    file->write(reinterpret_cast<const char*>(&values_size), sizeof(int32_t));

    // 将压缩后的timestamp 写入文件
    file->write(compress_timestamp.data(), timestamps_size);
    // 将压缩后的val 写入文件
    file->write(compress_val.data(), values_size);
//    // 将时间戳写入文件
//    size = sizeof(high_resolution_clock::time_point);
//    for (const auto & timestamp : data_block->m_timestamps)
//    {
//        file->write(reinterpret_cast<const char*>(&timestamp), size);
//    }

//    // 将数据写入文件
//    if (data_block->m_type == DataBlock::Type::DATA_STRING)
//    {
//        size = sizeof(uint16_t);
//        for (const string & value : data_block->m_values)
//        {
//            auto length = static_cast<uint16_t>(value.length());
//            file->write(reinterpret_cast<const char*>(&length), size);  // 写大小
//            file->write(value.c_str(), length);  // 写字符串
//        }
//
//        file->flush();
//        m_file_manager.release_file_stream(file_path);
//        return data_block->m_size + 4 * data_block->m_values.size();
//    }
//    else if (data_block->m_type == DataBlock::Type::DATA_FLOAT)
//    {
//        float data;
//        size = sizeof(float);
//        for (const auto & value : data_block->m_values)
//        {
//            data = stof(value);
//            file->write(reinterpret_cast<const char*>(&data), size);
//        }
//    }
//    else if (data_block->m_type == DataBlock::Type::DATA_INTEGER)
//    {
//        int data;
//        size = sizeof(int);
//        for (const auto & value : data_block->m_values)
//        {
//            data = stoi(value);
//            file->write(reinterpret_cast<const char*>(&data), size);
//        }
//    }
//    else
//    {
//        std::cerr << "Error : Unknown type - m_from engine/tsm/tsm.cpp" << std::endl;
//    }

    file->flush();
    m_file_manager.release_file_stream(file_path);
    return timestamps_size + values_size;
}

bool TSM::read_data_from_file(
        std::shared_ptr<DataBlock> & data_block,
        const string & file_path,
        int64_t offset)
{
    auto file = m_file_manager.get_file_stream(file_path);
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for reading - m_from engine/tsm_/tsm_.h" << std::endl;
        return false;
    }
    file->seekg(offset);

    // 读取类型 长度
    file->read(reinterpret_cast<char*>(&data_block->m_type), sizeof(data_block->m_type));
    file->read(reinterpret_cast<char*>(&data_block->m_length), sizeof(data_block->m_length));

    int size;

    // 读取时间戳
    data_block->m_timestamps.clear();
    auto num = data_block->m_length;
    size = sizeof(high_resolution_clock::time_point);
    for (size_t i = 0; i < num; ++i)
    {
        high_resolution_clock::time_point timestamp;
        file->read(reinterpret_cast<char*>(&timestamp), size);
        data_block->m_timestamps.push_back(timestamp);
    }

    // 读取值
    data_block->m_values.clear();
    if (data_block->m_type == DataBlock::Type::DATA_STRING)
    {
        for (size_t i = 0; i < num; ++i)
        {
            uint16_t length;
            file->read(reinterpret_cast<char*>(&length), sizeof(uint16_t));
            if (file->good())
            {
                char buffer[length + 1];
                file->read(buffer, length);
                buffer[length] = '\0';
                data_block->m_values.push_back(buffer);
            }
        }
    }
    else if (data_block->m_type == DataBlock::Type::DATA_INTEGER)
    {
        int value;
        string data;
        size = sizeof(int);
        for (size_t i = 0; i < num; ++i)
        {
            file->read(reinterpret_cast<char*>(&value), size);
            data = std::to_string(value);
            data_block->m_values.push_back(data);
        }
    }
    else if (data_block->m_type == DataBlock::Type::DATA_FLOAT)
    {
        float value;
        string data;
        size = sizeof(float);
        for (size_t i = 0; i < num; ++i)
        {
            file->read(reinterpret_cast<char*>(&value), size);
            data = std::to_string(value);
            data_block->m_values.push_back(data);
        }
    }
    else
    {
        std::cerr << "Error : Unknown type - m_from /engin/tsm/tsm.cpp" << std::endl;
    }
    m_file_manager.release_file_stream(file_path);
    return true;
}

uint64_t TSM::write_index_entry_to_file(
        const std::shared_ptr<IndexEntry> & index_entry,
        const string & file_path,
        int64_t offset)
{
    auto file = m_file_manager.get_file_stream(file_path);
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for writing - m_from engine/tsm_/tsm_.h" << std::endl;
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
    auto file = m_file_manager.get_file_stream(file_path);
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for reading - m_from engine/tsm_/tsm_.h" << std::endl;
        return false;
    }
    file->seekg(offset);
    m_file_manager.release_file_stream(file_path);
}

bool TSM::write_index_meta_to_file(
        const std::shared_ptr<IndexBlockMeta> & index_meta,
        const string & file_path,
        int64_t offset)
{
    auto file = m_file_manager.get_file_stream(file_path);
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for writing - m_from engine/tsm_/tsm_.h" << std::endl;
        return false;
    }
    file->seekp(offset);

    size_t size = 12;
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
    auto file = m_file_manager.get_file_stream(file_path);
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for reading - m_from engine/tsm_/tsm_.h" << std::endl;
        return false;
    }
    file->seekg(offset);

    uint16_t key_size;
    file->read(reinterpret_cast<char*>(&key_size), sizeof(uint16_t));

    char buffer[key_size + 1];
    file->read(buffer, key_size);
    buffer[key_size] = '\0';

    IndexBlockMeta::Type type;
    file->read(reinterpret_cast<char*>(&type), sizeof(IndexBlockMeta::Type));

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
        const string & file_path)
{
    auto file = m_file_manager.get_file_stream(file_path);
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for reading - m_from engine/tsm_/write.cpp" << std::endl;
        return false;
    }

    file->seekp(8 + 4 * 1024 * 1024);  // 跳转到文件末尾

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
    auto file = m_file_manager.get_file_stream(file_path);
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for reading - m_from engine/tsm_/write.cpp" << std::endl;
        return false;
    }
    file->seekg(-8, std::ios::end);  // 跳转到Footer 开头
    file->read(reinterpret_cast<char*>(&footer), sizeof(footer));

    m_file_manager.release_file_stream(file_path);
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
        IndexBlockMeta::Type type,
        const string & measurement,
        const string & field)
{
    string series_key = measurement + field;
    auto length = static_cast<uint16_t>(series_key.length());

    switch (type) {
        case IndexBlockMeta::DATA_INTEGER:
        {
            std::shared_ptr<IndexBlockMeta> e(new IndexBlockMeta(length, series_key, IndexBlockMeta::DATA_INTEGER));
            return e;
        }
        case IndexBlockMeta::DATA_STRING:
        {
            std::shared_ptr<IndexBlockMeta> e(new IndexBlockMeta(length, series_key, IndexBlockMeta::DATA_STRING));
            return e;
        }
        case IndexBlockMeta::DATA_FLOAT:
        {
            std::shared_ptr<IndexBlockMeta> e(new IndexBlockMeta(length, series_key, IndexBlockMeta::DATA_FLOAT));
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
    auto file = m_file_manager.get_file_stream(file_path);
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