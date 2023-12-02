#include <iostream>

#include <engine/tsm/skip_list.h>
#include <engine/tsm/tsm.h>
using namespace dt::tsm;

#include <fstream>
#include <random>


// 生成一个随机时间戳
std::chrono::system_clock::time_point random_time()
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();  // 获取当前时间点
    // 创建随机数生成器
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 86400);  // 定义时间范围（比如，24小时 = 86400秒）
    int random_seconds = dis(gen);  // 生成一个随机秒数

    // 添加这些秒数到当前时间点
    std::chrono::system_clock::time_point random_time_point = now + std::chrono::seconds(random_seconds);

    // 将时间点转换为可读的时间格式（例如：C time）
    time_t random_time = std::chrono::system_clock::to_time_t(random_time_point);
    std::cout << "Random time point: " << std::ctime(&random_time);
    return random_time_point;
}

int main()
{
    SkipList<int> sl;
    // 向跳表插入随机时间戳和节点
    for (int i = 0; i < 100; ++i)
    {
        int a= rand();
        sl.put(random_time(), a);
    }
    for (auto it = sl.begin(); it != sl.end(); ++it)
    {

    }

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
