#include <iostream>

#include <engine/tsm/tsm.h>
using namespace dt::tsm;

#include <fstream>

int main()
{
    // int
//    DataBlock<int> dataBlock(DataBlock<int>::DATA_INTEGER);
//    dataBlock.write(high_resolution_clock::now(), 42);
//    dataBlock.write(high_resolution_clock::now(), 100);
//    dataBlock.setLength(2);

    // float
//    DataBlock<float> dataBlock(DataBlock<float>::DATA_FLOAT);
//    dataBlock.write(high_resolution_clock::now(), 23.1);
//    dataBlock.write(high_resolution_clock::now(), 29.3);
//    dataBlock.write(high_resolution_clock::now(), 22.9);
//    dataBlock.setLength(3);

    // string
    DataBlock<string> dataBlock_01(DataBlock<string>::DATA_STRING);
    dataBlock_01.write(high_resolution_clock::now(), "hello");
    dataBlock_01.write(high_resolution_clock::now(), "world");
    dataBlock_01.setLength(2);

    DataBlock<string> dataBlock_02(DataBlock<string>::DATA_STRING);
    dataBlock_02.write(high_resolution_clock::now(), "DTimeDB");
    dataBlock_02.write(high_resolution_clock::now(), "test");
    dataBlock_02.write(high_resolution_clock::now(), "tsm");
    dataBlock_02.setLength(3);

    Json json = dataBlock_01.json();
    std::ofstream ofs("./../tsm.json");
    ofs << json.str();
    ofs.close();

    TSM tsm;
    std::string file_path = "data.tsm";
    Header header(520, 1);


    tsm.write_header_to_file(header, file_path);
    // 数据写入文件
    auto block_01 = tsm.write_data_string_to_file(dataBlock_01, file_path);
    auto block_02 = tsm.write_data_string_to_file(dataBlock_02, file_path);

    std::cout << block_01 << std::endl;
    std::cout << block_02 << std::endl;

    Footer footer(8 + block_01 + block_02);  // 尾部偏移量
    tsm.write_footer_to_file(footer, file_path);

    // 从文件中读取数据
    Header readHeader{};
    Footer readFooter{};
    DataBlock<string> readDataBlock;
    if (tsm.read_header_from_file(readHeader, file_path))
    {
        std::cout << readHeader.json() << std::endl;
    }
    for (int i = 0; i < 2; ++i)
    {
        if (tsm.read_data_string_from_file(readDataBlock, file_path))
        {
            std::cout << "Data read from file successfully." << std::endl;
            std::cout << readDataBlock.json() << std::endl;
        }
        else
        {
            std::cerr << "Failed to read data from file." << std::endl;
        }
    }
    if (tsm.read_footer_from_file(readFooter, file_path))
    {
        std::cout << readFooter.json() << std::endl;
    }
    return 0;
}