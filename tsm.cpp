#include <iostream>

#include <engine/tsm/header.h>
using namespace dt::tsm;

int main()
{
    Controller<int> controller;

    // 向跳表插入随机时间戳和节点
    for (int i = 0; i < 100; ++i)
    {
        int num = rand();
        controller.write(Tool::random_time(false), num);
    }

//    Json json = dataBlock_01.json();
//    std::ofstream ofs("./../tsm.json");
//    ofs << json.str();
//    ofs.close();

//    TSM tsm;
//    std::string file_path = "data.tsm";
//    Header header(520, 1);
//
//    int64_t offset_head = 8;  // 头部偏移量(默认带头)
//
//    tsm.write_header_to_file(header, file_path);
    // 数据写入文件
//    auto block_01 = tsm.write_data_to_file(dataBlock, file_path);
//    auto block_02 = tsm.write_data_string_to_file(dataBlock_02, file_path);
//    std::cout << "Size:" << block_01 << std::endl;
//    std::cout << block_02 << std::endl;

//    Footer footer(8 + block_01);  // 尾部偏移量
//    tsm.write_footer_to_file(footer, file_path);

    // 从文件中读取数据
//    Header readHeader{};
//    Footer readFooter{};
//    DataBlock<int> readDataBlock;
//    if (tsm.read_header_from_file(readHeader, file_path))
//    {
//        std::cout << readHeader.json() << std::endl;
//    }
//    for (int i = 0; i < 10; ++i)
//    {
//        if (tsm.read_data_from_file(readDataBlock, file_path))
//        {
//            std::cout << "Data read from file successfully." << std::endl;
//            std::cout << readDataBlock.json() << std::endl;
//        }
//        else
//        {
//            std::cerr << "Failed to read data from file." << std::endl;
//        }
//    }
//    if (tsm.read_footer_from_file(readFooter, file_path))
//    {
//        std::cout << readFooter.json() << std::endl;
//    }
//    return 0;
}
