#include "engine/tsm/tsm_ingredient.h"
#include "file_manager/file_io_manager.h"
#include <fstream>
#include <iostream>
int main()
{
    dt::file::FileIOManager io(3);
    dt::tsm::IndexBlockMeta index_meta;
    auto file = io.get_file_stream("./../dbs/db_test/school-1709440419491139069.tsm" , "binary");
    if (!file->is_open())
    {
        std::cerr << "Error: Could not open file for writing - m_from engine/tsm/tsm_.h" << std::endl;
        return 0;
    }
//    file->seekp(-8, std::ios::end);
//    int64_t xxx = 432;
//    file->write(reinterpret_cast<const char *>(&xxx), sizeof(int64_t));

    file->seekg(-8, std::ios::end);
    auto a = file->tellp();
    auto b = file->tellg();
    std::cout <<a << std::endl;
    std::cout <<b << std::endl;
    int64_t offset = 0;
    file->read(reinterpret_cast<char*>(&offset), sizeof(int64_t));
//

//    file->seekp(tail_offset - 8);  // 跳转到文件末尾
//    file->write(reinterpret_cast<const char*>(&footer.m_offset), sizeof(footer.m_offset));







    //  file->seekp(offset);
    //
    //    int64_t size = 12;


    //    auto type = index_meta->get_type();
    //    file->write(reinterpret_cast<const char*>(&type), sizeof(index_meta->get_type()));
    //    uint16_t count = index_meta->get_count();
    //    file->write(reinterpret_cast<const char*>(&count), sizeof(uint16_t));
    //
    //    file->flush();
    //    m_file_manager.release_file_stream(file_path);
    //    return size + length;
}