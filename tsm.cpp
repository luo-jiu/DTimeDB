#include <iostream>

#include <engine/tsm/header.h>
using namespace dt::tsm;

string file_path  = "data.tsm";

int main()
{
    SkipList<string> sl;
    TSM tsm;

    Header header(520, 1);
    std::shared_ptr<DataBlock> block(new DataBlock());
    Footer footer;

    // 写入数据
    block->m_type = DataBlock::Type::DATA_STRING;

    Tool::write_float_skip_list(sl, 10, true);
    Tool::transfer(sl, block, true);
    tsm.write_header_to_file(header, file_path);
    tsm.write_data_to_file(block, file_path, 0);

    // 读取数据
    std::shared_ptr<DataBlock> new_block(new DataBlock());
    tsm.read_data_from_file(new_block, "data.tsm" , 0);
    std::cout << new_block->json() << std::endl;


//    string measurement = "school,class=高一2班";
//    string field = "温度";
//    string file_path = "data.tsm";
//    Controller<string> controller;
//    SkipList<string> sl;


    // 向跳表插入随机时间戳和节点
//    for (int i = 0; i < 100; ++i)
//    {
//        int num = rand();
//        string str = "xxx";
//        sl.put(Tool::random_time(false), str);
//        controller.write(Tool::random_time(false), "xxx");
//    }

//    controller.write_file();

//    Json json = dataBlock_01.json();
//    std::ofstream ofs("./../tsm_.json");
//    ofs << json.str();
//    ofs.close();

//    TSM tsm_;
//    std::string file_path = "data.tsm_";
//    Header header(520, 1);
//
//    int64_t offset_head = 8;  // 头部偏移量(默认带头)
//
//    tsm_.write_header_to_file(header, file_path);

//    Footer footer(8 + block_01);  // 尾部偏移量
//    tsm_.write_footer_to_file(footer, file_path);

    // 从文件中读取数据
//    Header readHeader{};
//    Footer readFooter{};
//    DataBlock<int> readDataBlock;
//    if (tsm_.read_header_from_file(readHeader, file_path))
//    {
//        std::cout << readHeader.json() << std::endl;
//    }
//    for (int i = 0; i < 10; ++i)
//    {
//        if (tsm_.read_data_from_file(readDataBlock, file_path))
//        {
//            std::cout << "Data read from file successfully." << std::endl;
//            std::cout << readDataBlock.json() << std::endl;
//        }
//        else
//        {
//            std::cerr << "Failed to read data from file." << std::endl;
//        }
//    }
//    if (tsm_.read_footer_from_file(readFooter, file_path))
//    {
//        std::cout << readFooter.json() << std::endl;
//    }
//    return 0;
}
