//
// Created by luojiu on 23-9-19.
//
#include "page_block.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <memory>

const int PAGE_SIZE = 1024 * 4;  // 4kb
static int count_page = 0;  // number of page
static long page_offset = 0;  // page offset

/**
 * 其实这个函数的作用就是直接在内存中写好一块4kb大小，返回指针
 * @param path 上一页在哪个文件
 * @param offset 上一页对于path的偏移量
 * @return 返回该块的头指针
 */
std::pair<std::ofstream, std::ifstream> get_new_page(const std::string& path, std::streampos offset) {
    /*
     * 一切的前提：得开一个文件，我们称之为区
     * 一个区有很多的页(暂时不定有多少，反正就是有很多)
     *
     * 获取新的页其实就是在区的最后，追加一个新的4kb的页的header和ender，将文件指针移动到
     * 页的header之后传给插入数据的函数(bool write_page)
     */

    // 通过传入的参数确定之前文件的位置，拿到输入输出流
    std::ifstream old_input(path, std::ios::binary);
    std::ofstream old_output(path, std::ios::binary);

    old_input.seekg(offset);  // 一来先跳转指针到指定位置
//    std::streampos old_head_index = old_input.tellg();  // 老页开头的偏移量
    old_input.seekg(0, std::ios::end);  // 跳转到文件末尾
    std::streampos pos = old_input.tellg();  //  获取文件末尾位置

    old_input.close();
    // 假设从一个不会重名的函数那里获取到的文件路径
    std::ofstream output_file("area.bin", std::ios::binary);
    std::ifstream input_file("area.bin", std::ios::binary);
    std::streampos new_head_index = output_file.tellp();  // 新页开头的偏移量
    output_file.seekp(0, std::ios::end);
    input_file.seekg(0, std::ios::end);


    Header old_header{};
    Ender old_ender{};

    Header new_header{};  // 创建页头
    Ender new_ender{"abcdef",static_cast<std::streampos>(-1)};  // 创建页尾


    std::cout << "是否有数据:" << pos-offset << std::endl;
    if(!(pos - offset) || (pos - offset) == -1){// 说明前一页文件为空(这里一次就完成了新页创建)
        // 前一页为空，说明单纯来拿<新页>的，不需要连接！复制完头和尾直接返回

        // 直接手动赋值即可，不需要移动文件指针
        new_header.prev.offset = static_cast<std::streampos>(-1);  // 上一页没数据
        strcpy(new_header.path, "area.bin");  // 附带上新页创建的位置
        output_file.write(reinterpret_cast<char*>(&new_header), sizeof(Header));  // 刷入数据到 新页

        const int body_size = PAGE_SIZE - sizeof(Header) - sizeof(Ender);
        std::cout<<body_size<<std::endl;
        output_file.seekp(body_size, std::ios::beg);  // 跳过数据部分
        output_file.write(reinterpret_cast<const char*>(&new_ender), sizeof(Ender));  // 刷入数据到 新页

        output_file.seekp(new_head_index);  // 指针回到开头，新页创建成功

        output_file.flush();
        // 释放资源
//        old_input.close();
//        output_file.close();
//        return;
        return std::make_pair(std::move(output_file), std::move(input_file));  // 返回<新页>的输入输出流
    }

    // 下面是有<前一页>的情况，需要连接前后页
    std::streampos old_head_index = old_input.tellg();  // 记录前一页文件开头位置
    old_input.read(reinterpret_cast<char*>(&old_header), sizeof(old_header));  // 获取头部信息
    std::streampos old_end_index = (static_cast<std::streampos>(PAGE_SIZE-sizeof(Ender)) + old_head_index);  // 获取尾部开头位置

    old_input.seekg(old_end_index);  // 跳转到尾部开头
    old_input.read(reinterpret_cast<char*>(&old_ender), sizeof(Ender));  // 获取尾部信息

    old_output.seekp(old_end_index);  // 输出流移动到尾部开头，为了给连接下一页刷取数据做准备！

//    output_file.seekp(0, std::ios::end);
//    std::streampos pos = output_file.tellp();

    /*
     * 这个区的首页，需要记录head，上一页的值path offset直接拷贝即可
     *  +-----+  <---h  +-----+
     *  | old |         | new |
     *  +-----+         +-----+
     */
    strcpy(new_header.prev.path, old_header.path);  // 记录上一页文件路径
    new_header.prev.offset = old_header.prev.offset;  // 记录上一页所在的偏移量
    output_file.write(reinterpret_cast<char*>(&new_header), sizeof(Header));  // 刷入数据到 新页

    const int body_size = PAGE_SIZE - sizeof(Header) - sizeof(Ender);
    output_file.seekp(body_size, std::ios::cur);  // 跳过数据部分
    output_file.write(reinterpret_cast<const char*>(&new_ender), sizeof(Ender));  // 刷入数据到 新页

    output_file.seekp(new_head_index);  // 指针回到开头，新页创建成功

    /*
     * 还需要将上个页的尾部数据修改，使其与新的页建立连接
     *  +-----+         +-----+
     *  | old |         | new |
     *  +-----+  e--->  +-----+
     */
    strcpy(old_ender.next.path, new_header.path);
    old_ender.next.offset = output_file.tellp();  // 追加写本来就在文件末尾，末尾即是开头
    old_output.write(reinterpret_cast<char*>(&old_ender), sizeof(Ender));  // 刷入新数据到 老页
    old_output.flush();

    // 关闭资源
    old_input.close();
    old_output.close();

    return std::make_pair(std::move(output_file), std::move(input_file));  // 返回输入输出流
}



/**
 *
 * 此函数 会直接在一块已经申请好的4kb大小的页上，跳过头找到末尾直接开始写入
 * 写完之后再将这块内存重新刷写回磁盘
 *
 * @param output_file 拿来写入数据
 * @param input_file 拿来读取头部信息，方便链表连接
 * @param data 待写入的数据
 * @param num 数组大小
 * @return
 */
bool write_page(std::ofstream&& output_file, std::ifstream&& input_file, char* data[], int num) {
    /*
     * 先判断还有没有空闲位置
     * 因为还没定下架构，这里的页都未构建索引
     *
     */

    // 先读取头和尾部
    Header now_header={};
    Ender now_ender={};

    std::streampos head_index = input_file.tellg();  // 记录页开头位置
    input_file.read(reinterpret_cast<char*>(&now_header), sizeof(Header));  // 读取头部信息
    int total_lines = now_header.num_lines;  // 总行数
    std::streampos body_index = input_file.tellg();  // 记录数据开头位置
    int use_size = total_lines * sizeof(Data_line);  // 计算使用了多少

    std::streampos end_index = (head_index + static_cast<std::streampos>(PAGE_SIZE-sizeof(Ender)));
    input_file.seekg(end_index);
    input_file.read(reinterpret_cast<char*>(&now_ender), sizeof(Ender));  // 读取尾部信息
    input_file.close();

    // 假设有数据，则尝试跳过
    if (total_lines > 0){
        body_index += use_size;
    }
    output_file.seekp(body_index);  // 输出流跳转到指定位置

    //每次写入的时候都要判断是否超过写入上限，超过则另开一块页，两页通过 偏移量offset 相连相连
    int free_size = PAGE_SIZE - sizeof(Header) - sizeof(Ender) - use_size;
    int one_page_line_num = 0;  // 用来记录每一页新插入的行数量，方便计算空间是否足够
    for(int i = 0; i < num; ++i) {
        Data_line line;
        line.next_line_offset = static_cast<uint16_t>(sizeof(Data_line));  // 设置行偏移量<!!!这里有问题，我存的是全数据大小，而不是数据开头到下一段数据的偏移量!!!>
        strcpy(line.data, data[i]);  // 设置行数据

        // 每次插入前判断
        if(static_cast<std::streamoff>(one_page_line_num * sizeof(Data_line)) < free_size) {  // 空间足够
            // 写入数据
            output_file.write(reinterpret_cast<const char*>(&line), sizeof(Data_line));
            ++total_lines;  // 行总数增加
            ++one_page_line_num;
        } else {
            auto streams = get_new_page("area.bin", head_index);  // 获取到新页
            std::ofstream& new_output = streams.first;
            std::ifstream& new_input = streams.second;

            output_file.seekp(head_index);  // 跳转到开头
            now_header.num_lines = total_lines;  // 更新总数
            output_file.write(reinterpret_cast<const char*>(&now_header), sizeof(Header));  //写入磁盘

            total_lines = 0;  // 重置新页总数
            one_page_line_num = 0;   // 重置计数器

            output_file.close();
            output_file = std::move(new_output);  // 跳转到新页，继续重新开始读写
        }
    }

    // 刷新头信息
    now_header.num_lines = total_lines;
    output_file.seekp(head_index);
    output_file.write(reinterpret_cast<const char*>(&now_header), sizeof(Header));  // 重新刷入头部
    output_file.flush();

    output_file.close();
    std::cout << "落盘结束..." << std::endl;
}






/**
 *
 * 暂时实现一个全数据遍历(只通过 页链表page_linklist 和 行偏移line_offset)
 *
 * @param path
 * @return
 */
void read_page(const std::string& path, std::streampos offset) {
    std::ifstream input_file(path, std::ios::binary);  // 获取输入流

    if(input_file.is_open()) {  // 判断文件是否成功被打开
        input_file.seekg(offset);  // 跳转到指定位置
//        std::streampos head_index = offset;  // head的偏移量

        Header header{};
//        Ender ender{};
        Data_line line{};

        if(input_file.read(reinterpret_cast<char *>(&header), sizeof(Header))) {
            int num = header.num_lines;
            for (int i = 0; i < num; ++i) {  // 开始读取数据
                if(input_file.read(reinterpret_cast<char *>(&line), sizeof(Data_line))) {
                    std::cout << "Data:" << line.data << std::endl;
                    // 不需要跳转直接读取即可，因为大小一致还连续，不要太过分了
                }else{
                    std::cout << "Error: Linkage error with data row pointer" << path << std::endl;
                }
            }
        }else{  // 读取失败报错并退出
            std::cout << "Error: File is empty or here is an offset error." << path << std::endl;
            return;
        }
    }else{
        std::cout << "Error: File could not be opened." << path << std::endl;
    }
}

