#include <engine/tsm/tsm.h>
using namespace dt::tsm;

/**
 * 将TSM Header 写入到文件
 */
bool TSM::write_header_to_file(const Header & header, const string & filename)
{
    auto & file = FileManager::get_output_stream(filename);
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
bool TSM::read_header_from_file(Header & header, const string & filename)
{
    auto & file = FileManager::get_input_stream(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file for reading - from engine/tsm/tsm.h" << std::endl;
        return false;
    }

    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    return true;
}