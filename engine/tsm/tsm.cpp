#include <engine/tsm/tsm.h>
using namespace dt::tsm;

/**
 * 将TSM Header 写入到文件
 */
bool TSM::write_header_to_file(const Header & header, const string & file_path)
{
    auto & file = FileManager::get_output_stream(file_path);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file for writing - from engine/tsm_/write.cpp" << std::endl;
        return false;
    }
    file.write(reinterpret_cast<const char*>(&header), sizeof(header));

    file.flush();
    return true;
}

/**
 * 将TSM Header 读取到内存
 */
bool TSM::read_header_from_file(Header & header, const string & file_path)
{
    auto & file = FileManager::get_input_stream(file_path);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file for reading - from engine/tsm_/write.cpp" << std::endl;
        return false;
    }

    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    return true;
}

u_int64_t TSM::write_data_to_file(
        const std::shared_ptr<DataBlock> & data_block ,
        const string & file_path,
        int64_t offset) const
{
    auto & file = FileManager::get_output_stream(file_path);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file for writing - from engine/tsm_/tsm_.h" << std::endl;
        return -1;
    }
    file.seekp(offset);  // 移动到指定位置

    int _size;

    // 将类型 长度写入文件
    file.write(reinterpret_cast<const char*>(&data_block->m_type), sizeof(data_block->m_type));
    file.write(reinterpret_cast<const char*>(&data_block->m_length), sizeof(data_block->m_length));

    // 将时间戳写入文件
    _size = sizeof(high_resolution_clock::time_point);
    for (const auto & timestamp : data_block->m_timestamps)
    {
        file.write(reinterpret_cast<const char*>(&timestamp), _size);
    }

    // 将数据写入文件
    if (data_block->m_type == DataBlock::Type::DATA_STRING)
    {
        _size = sizeof(u_int16_t);
        for (const string & value : data_block->m_values)
        {
            auto length = static_cast<u_int16_t>(value.length());
            file.write(reinterpret_cast<const char*>(&length), _size);  // 写大小
            file.write(value.c_str(), length);  // 写字符串
        }

        file.flush();
        return data_block->m_size + 4 * data_block->m_values.size();
    }
    else if (data_block->m_type == DataBlock::Type::DATA_FLOAT)
    {
        float _data;
        _size = sizeof(float);
        for (const auto & value : data_block->m_values)
        {
            _data = stof(value);
            file.write(reinterpret_cast<const char*>(&_data), _size);
        }
    }
    else if (data_block->m_type == DataBlock::Type::DATA_INTEGER)
    {
        int _data;
        _size = sizeof(int);
        for (const auto & value : data_block->m_values)
        {
            _data = stoi(value);
            file.write(reinterpret_cast<const char*>(&_data), _size);
        }
    }
    else
    {
        std::cerr << "Error : Unknown type - from engine/tsm/tsm.cpp" << std::endl;
    }

    file.flush();
    return data_block->m_size;
}

bool TSM::read_data_from_file(
        std::shared_ptr<DataBlock> & data_block,
        const string & file_path,
        int64_t offset)
{
    auto & file = FileManager::get_input_stream(file_path);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file for reading - from engine/tsm_/tsm_.h" << std::endl;
        return false;
    }
    file.seekg(offset);

    // 读取类型 长度
    file.read(reinterpret_cast<char*>(&data_block->m_type), sizeof(data_block->m_type));
    file.read(reinterpret_cast<char*>(&data_block->m_length), sizeof(data_block->m_length));

    int _size;

    // 读取时间戳
    data_block->m_timestamps.clear();
    auto num = data_block->m_length;
    _size = sizeof(high_resolution_clock::time_point);
    for (size_t i = 0; i < num; ++i)
    {
        high_resolution_clock::time_point timestamp;
        file.read(reinterpret_cast<char*>(&timestamp), _size);
        data_block->m_timestamps.push_back(timestamp);
    }

    // 读取值
    data_block->m_values.clear();
    if (data_block->m_type == DataBlock::Type::DATA_STRING)
    {
        for (size_t i = 0; i < num; ++i)
        {
            u_int16_t length;
            file.read(reinterpret_cast<char*>(&length), sizeof(u_int16_t));
            if (file.good())
            {
                char buffer[length + 1];
                file.read(buffer, length);
                buffer[length] = '\0';
                data_block->m_values.push_back(buffer);
            }
        }
    }
    else if (data_block->m_type == DataBlock::Type::DATA_INTEGER)
    {
        int _value;
        string _data;
        _size = sizeof(int);
        for (size_t i = 0; i < num; ++i)
        {
            file.read(reinterpret_cast<char*>(&_value), _size);
            _data = std::to_string(_value);
            data_block->m_values.push_back(_data);
        }
    }
    else if (data_block->m_type == DataBlock::Type::DATA_FLOAT)
    {
        float _value;
        string _data;
        _size = sizeof(float);
        for (size_t i = 0; i < num; ++i)
        {
            file.read(reinterpret_cast<char*>(&_value), _size);
            _data = std::to_string(_value);
            data_block->m_values.push_back(_data);
        }
    }
    else
    {
        std::cerr << "Error : Unknown type - from /engin/tsm/tsm.cpp" << std::endl;
    }

    return true;
}

u_int64_t TSM::write_index_entry_to_file(
        const std::shared_ptr<IndexEntry> & index_entry,
        const string & file_path,
        int64_t offset) const
{
    auto & file = FileManager::get_output_stream(file_path);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file for writing - from engine/tsm_/tsm_.h" << std::endl;
        return false;
    }
    file.seekp(offset);

    long timestamp_size = sizeof(high_resolution_clock::time_point);
    file.write(reinterpret_cast<const char*>(&index_entry->get_max_time()), timestamp_size);
    file.write(reinterpret_cast<const char*>(&index_entry->get_min_time()), timestamp_size);

    int64_t _offset = index_entry->get_offset();
    file.write(reinterpret_cast<const char*>(&_offset), sizeof(int64_t));

    int64_t _size = index_entry->get_size();
    file.write(reinterpret_cast<const char*>(&_size), sizeof(int32_t));

    file.flush();
    return true;
}

bool TSM::read_index_entry_from_file(
        std::shared_ptr<IndexEntry> & index_entry,
        const string & file_path,
        int64_t offset)
{
    auto & file = FileManager::get_input_stream(file_path);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file for reading - from engine/tsm_/tsm_.h" << std::endl;
        return false;
    }
    file.seekg(offset);

}

bool TSM::write_index_meta_to_file(
        const std::shared_ptr<IndexBlockMeta> & index_meta,
        const string & file_path,
        int64_t offset) const
{
    auto & file = FileManager::get_output_stream(file_path);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file for writing - from engine/tsm_/tsm_.h" << std::endl;
        return false;
    }
    file.seekp(offset);

    size_t size = 12;
    u_int16_t _length = index_meta->get_key_length();
    file.write(reinterpret_cast<const char*>(&_length), sizeof(u_int16_t));
    file.write(index_meta->get_key().c_str(), _length);  // 写字符串
    auto _type = index_meta->get_type();
    file.write(reinterpret_cast<const char*>(&_type), sizeof(index_meta->get_type()));
    u_int16_t _count = index_meta->get_count();
    file.write(reinterpret_cast<const char*>(&_count), sizeof(u_int16_t));

    file.flush();
    return size + _length;
}

bool TSM::read_index_meta_from_file(
        std::shared_ptr<IndexBlockMeta> & index_meta,
        const string & file_path,
        int64_t offset)
{
    auto & file = FileManager::get_input_stream(file_path);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file for reading - from engine/tsm_/tsm_.h" << std::endl;
        return false;
    }
    file.seekg(offset);

    u_int16_t _key_size;
    file.read(reinterpret_cast<char*>(&_key_size), sizeof(u_int16_t));

    char buffer[_key_size + 1];
    file.read(buffer, _key_size);
    buffer[_key_size] = '\0';

    IndexBlockMeta::Type _type;
    file.read(reinterpret_cast<char*>(&_type), sizeof(IndexBlockMeta::Type));

    u_int16_t _count;
    file.read(reinterpret_cast<char*>(&_count), sizeof(u_int16_t));

    index_meta->set_key_length(_key_size);
    index_meta->set_key(buffer);
    index_meta->set_type(_type);
    index_meta->set_count(_count);

    return true;
}

/**
 * 将TSM Footer 写到末尾
 */
bool TSM::write_footer_to_file(const Footer & footer, const string & file_path)
{
    auto & file = FileManager::get_output_stream(file_path);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file for reading - from engine/tsm_/write.cpp" << std::endl;
        return false;
    }
    file.seekp(0, std::ios::end);  // 跳转到文件末尾
    file.write(reinterpret_cast<const char*>(&footer), sizeof(footer));

    file.flush();
    return true;
}

/**
 * 将TSM Footer 读取出来
 */
bool TSM::read_footer_from_file(Footer & footer, const string & file_path)
{
    auto & file = FileManager::get_input_stream(file_path);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file for reading - from engine/tsm_/write.cpp" << std::endl;
        return false;
    }
    file.seekg(-8, std::ios::end);  // 跳转到Footer 开头
    file.read(reinterpret_cast<char*>(&footer), sizeof(footer));

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
    auto length = static_cast<u_int16_t>(series_key.length());

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
            std::cerr << "Error : Type '" << type << "' does not meet - from engine/tsm_/tsm_.cpp" << std::endl;
            return nullptr;
        }
    }
}