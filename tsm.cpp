#include <iostream>
#include <thread>

#include <engine/tsm/header.h>
using namespace dt::tsm;

string file_path  = "data.tsm";

int main()
{
    Write write;

//    std::thread monitor_thread(write.);
//    monitor_thread.detach();

    TSM tsm;
    Header header(20, 1);
    Footer footer;
    tsm.create_tsm(header, footer, file_path);  // 创建 tsm

    Tool::test_data(write, 100, true, false, file_path);  // 写入数据

    std::shared_ptr<DataBlock> block(new DataBlock());
//    Tool::write_integer_skip_list(sl, 100, false);
//    Tool::transfer(sl, block, true);
//    tsm.write_data_to_file(block, file_path, 8);

    // 读取数据
//    Header new_header;
//    std::shared_ptr<DataBlock> new_block(new DataBlock());
//    tsm.read_header_from_file(new_header, file_path);
//    std::cout << header.json() << std::endl;
//    tsm.read_data_from_file(new_block, file_path , 8);
//    std::cout << new_block->json() << std::endl;


//    string measurement = "school,class=高一2班";
//    string field = "温度";
//    string file_path = "data.tsm";
//    Controller<string> controller;
//    SkipList<string> sl;

//    Json json = dataBlock_01.json();
//    std::ofstream ofs("./../tsm_.json");
//    ofs << json.str();
//    ofs.close();
}
