#include <iostream>
#include <thread>

#include <engine/tsm/header.h>
using namespace dt::tsm;

string file_path  = "data.tsm";
string measurement = "school";
string file_name = "temperature";

int main()
{
    Write write(measurement);

    // 开启异步刷新线程
    TSM tsm;
    Header header(20, 1);
    Footer footer;
    tsm.create_tsm(header, footer, file_path);  // 创建 tsm

    std::thread flush_thread(&Write::flush_disk, &write);

    Tool::test_data(write, 5, true, false, file_name, file_path);  // 写入数据
//    Tool::test_data(write, 100, true, false, file_name, file_path);  // 写入数据
    std::this_thread::sleep_for(std::chrono::seconds(15));
    Tool::test_data(write, 5, true, false, file_name, file_path);  // 写入数据


    while (true)
    {

    }
    flush_thread.join();
}
