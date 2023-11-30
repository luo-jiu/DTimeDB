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
    DataBlock<string> dataBlock(DataBlock<string>::DATA_STRING);
    dataBlock.write(high_resolution_clock::now(), "hello");
    dataBlock.write(high_resolution_clock::now(), "world");
    dataBlock.setLength(2);

//    dataBlock.write_to_file("xxx");
//    dataBlock.read_from_file("yyy");

    TSM tsm;

    Json json = dataBlock.json();
    std::ofstream ofs("./../tsm.json");
    ofs << json.str();
    ofs.close();

    // 数据写入文件
    if (tsm.write_data_string_to_file(dataBlock, "data.tsm"))
    {
        std::cout << "Data written to file successfully." << std::endl;
    }
    else
    {
        std::cerr << "Failed to write data to file." << std::endl;
    }

    // 从文件中读取数据
    DataBlock<string> readDataBlock;
    if (tsm.read_data_string_from_file(readDataBlock, "data.tsm"))
    {
        std::cout << "Data read from file successfully." << std::endl;
        std::cout << readDataBlock.json() << std::endl;
    }
    else
    {
        std::cerr << "Failed to read data from file." << std::endl;
    }

    return 0;
}