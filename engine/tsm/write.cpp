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
        std::cerr << "Error: Could not open file for writing - from engine/tsm/write.cpp" << std::endl;
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
        std::cerr << "Error: Could not open file for reading - from engine/tsm/write.cpp" << std::endl;
        return false;
    }

    file.read(reinterpret_cast<char*>(&header), sizeof(header));
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
        std::cerr << "Error: Could not open file for reading - from engine/tsm/write.cpp" << std::endl;
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
        std::cerr << "Error: Could not open file for reading - from engine/tsm/write.cpp" << std::endl;
        return false;
    }
    file.seekg(-8, std::ios::end);  // 跳转到Footer 开头
    file.read(reinterpret_cast<char*>(&footer), sizeof(footer));

    return true;
}

/**
 * 创建IndexEntry
 */
IndexEntry & TSM::create_index_entry(
        high_resolution_clock::time_point max_time,
        high_resolution_clock::time_point min_time,
        int64_t offset,
        int32_t size)
{
    IndexEntry index_entry(max_time, min_time, offset, size);
    return index_entry;
}
