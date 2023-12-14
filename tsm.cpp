#include <iostream>

#include <engine/tsm/skip_list.h>
#include <engine/tsm/tsm.h>
using namespace dt::tsm;

//#include <fstream>
//#include <random>
//#include "engine/circularList/include/circular_list_header.h"
// //生成一个随机时间戳
//std::chrono::system_clock::time_point random_time()
//{
//    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();  // 获取当前时间点
//    // 创建随机数生成器
//    std::random_device rd;
//    std::mt19937 gen(rd());
//    std::uniform_int_distribution<> dis(0, 86400);  // 定义时间范围（比如，24小时 = 86400秒）
//    int random_seconds = dis(gen);  // 生成一个随机秒数
//
//    // 添加这些秒数到当前时间点
//    std::chrono::system_clock::time_point random_time_point = now + std::chrono::seconds(random_seconds);
//
////    // 将时间点转换为可读的时间格式（例如：C time）
////    time_t random_time = std::chrono::system_clock::to_time_t(random_time_point);
////    std::cout << "Random time point: " << std::ctime(&random_time);
//    return random_time_point;
//}
//
//int main()
//{
////    Timestamp timestamp;
////    time_t res= timestamp.getTimestamp();
////    time_t con=std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
////    if (res==con){
////        cout<<"right"<<endl;
////    } else{
////        cout<<"wrong"<<endl;
////    }
//
//    DataBlock<int> dataBlock(DataBlock<int>::DATA_INTEGER);
//
//    SkipList<int> sl;
//    // 向跳表插入随机时间戳和节点
//    for (int i = 0; i < 100; ++i)
//    {
//        int a= rand();
//        sl.put(random_time(), a);  // 向跳表插入元素
//    }
//    for (auto it = sl.begin(); it != sl.end(); ++it)  // 迭代器
//    {
//        if (it != sl.end())
//        {
//            auto key_value = *it;
//            dataBlock.write(key_value.first, key_value.second);
////            auto time = key_value.first;
////            time_t temp = system_clock::to_time_t(time);
////            std::cout << std::ctime(&temp);
////            std::cout << key_value.second << '\n';
//            dataBlock.m_length++;
//        }
//    }
//
//    // 最小时间戳
//    auto min = sl.min_key();
//
//    // 最大时间戳
//    auto max = sl.max_key();
//
//    time_t temp1 = system_clock::to_time_t(min);
//    time_t temp2 = system_clock::to_time_t(max);
//    std::cout << "=======================" << std::endl;
//    std::cout << sl.size() << std::endl;
//    std::cout << std::ctime(&temp1);
//    std::cout << std::ctime(&temp2);
//    std::cout << "=======================" << std::endl;
//
////    Json json = dataBlock_01.json();
////    std::ofstream ofs("./../tsm.json");
////    ofs << json.str();
////    ofs.close();
//
//    TSM tsm;
//    std::string file_path = "data.tsm";
//    Header header(520, 1);
//
//    tsm.write_header_to_file(header, file_path);
//    // 数据写入文件
//    auto block_01 = tsm.write_data_to_file(dataBlock, file_path);
////    auto block_02 = tsm.write_data_string_to_file(dataBlock_02, file_path);
//
//    std::cout << "Size:" << block_01 << std::endl;
////    std::cout << block_02 << std::endl;
//
//    Footer footer(8 + block_01);  // 尾部偏移量
//    tsm.write_footer_to_file(footer, file_path);
//
//    // 从文件中读取数据
//    Header readHeader{};
//    Footer readFooter{};
//    DataBlock<int> readDataBlock;
//    if (tsm.read_header_from_file(readHeader, file_path))
//    {
//        std::cout << readHeader.json() << std::endl;
//    }
//    for (int i = 0; i < 1; ++i)
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
//
//    return 0;
//}
