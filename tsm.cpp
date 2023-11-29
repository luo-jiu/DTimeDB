#include <iostream>

#include <engine/tsm/tsm.h>
using namespace dt::tsm;

#include <fstream>

int main()
{
    DataBlock<int> dataBlock(DataBlock<int>::DATA_INTEGER);
    dataBlock.write(high_resolution_clock::now(), 42);
    dataBlock.write(high_resolution_clock::now(), 100);
    dataBlock.setLength(2);

//    dataBlock.write_to_file("xxx");
//    dataBlock.read_from_file("yyy");

    Json json = dataBlock.json();
    std::ofstream ofs("./../tsm.json");
    ofs << json.str();
    ofs.close();

    // 数据写入文件
    if (dataBlock.write_to_file("data.tsm"))
    {
        std::cout << "Data written to file successfully." << std::endl;
    }
    else
    {
        std::cerr << "Failed to write data to file." << std::endl;
    }

    // 从文件中读取数据
    DataBlock<int> readDataBlock;
    if (readDataBlock.read_from_file("data.tsm"))
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