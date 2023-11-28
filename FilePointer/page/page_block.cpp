//
// Created by luojiu on 23-9-19.
//
#include "page_block.h"
#include <iostream>
#include <fstream>
#include <cstring>

const int PAGE_SIZE = 1024 * 4;  // 4kb
static int count_page = 0;  // number of page
static long page_offset = 0;  // page offset

/**
 * 其实这个函数的作用就是直接在内存中写好一块4kb大小，返回指针
 * @param now_header 需要连接的前一页
 * @param now_ender 需要连接的后一页
 * @return 返回该块的头指针
 */
std::streampos get_new_page(struct Header *now_header, struct Ender *now_ender) {
    /*
     * 一切的前提：得开一个文件，我们称之为区
     * 一个区有很多的页(暂时不定有多少，反正就是有很多)
     *
     * 获取新的页其实就是在区的最后，追加一个新的4kb的页的header和ender，将文件指针移动到
     * 页的header之后传给插入数据的函数(bool write_page)
     */
    std::ofstream output_file("area.bin",std::ios::app | std::ios::binary);  // default is null

    Header new_header{};  // 创建页头
    Ender new_ender{};  // 创建页尾

    output_file.seekp(0, std::ios::end);
    std::streampos pos = output_file.tellp();
    if(pos == 0){  // 文件为空
        // 这个区的首页，没有上一页，值为-1
        new_header.prev.offset = static_cast<std::streampos>(-1);
    } else {  // 文件不为空
        // 连接页与页
        strcpy(new_header.prev.path, "area.bin");
        now_ender->next.offset = (now_header->prev.offset+=(PAGE_SIZE));
        strcpy(new_header.prev.path, "area.bin");  // 记录文件位置
        new_header.prev.offset = (now_ender->next.offset-=(PAGE_SIZE-1));  // 往前移动4kb位置
    }
    std::streampos before_write_position = output_file.tellp();  // 记录写之前的偏移量

    const int body_size = PAGE_SIZE - sizeof(Header) - sizeof(Ender);

    std::cout << "Header size: " << sizeof(Header) << std::endl;
    std::cout << "Ender size: " << sizeof(Ender) << std::endl;

    output_file.write(reinterpret_cast<const char*>(&new_header), sizeof(Header));
    output_file.seekp(body_size, std::ios::cur);  // 跳过数据部分
    output_file.write(reinterpret_cast<const char*>(&new_ender), sizeof(Ender));

    output_file.close();  // 关闭

    return before_write_position;  // 返回位置
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
bool write_page(std::ofstream output_file, std::ifstream input_file, char* data[], int num) {
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
    int use_size = (total_lines*sizeof(Data_line));  // 计算使用了多少

    head_index += (PAGE_SIZE-sizeof(Ender));
    input_file.seekg(head_index);
    input_file.read(reinterpret_cast<char*>(&now_header), sizeof(Ender));  // 读取尾部信息

    // 假设有数据，则尝试跳过
    if (total_lines > 0){
        body_index += use_size;
    }
    output_file.seekp(body_index);  // 输出流跳转到指定位置

    //每次写入的时候都要判断是否超过写入上限，超过则另开一块页，两页通过 偏移量offset 相连相连
    int free_size = PAGE_SIZE - sizeof(Header) + sizeof(Ender) - use_size;
    int one_page_line_num = 0;  // 用来记录每一页新插入的行数量，方便计算空间是否足够
    for(int i = 0; i < num; ++i) {
        Data_line line;
        line.next_line_offset = static_cast<uint16_t>(sizeof(Data_line));  // 设置行偏移量
        strcpy(line.data, data[i]);  // 设置行数据

        // 每次插入前判断
        if(static_cast<std::streamoff>(one_page_line_num * sizeof(Data_line)) < free_size) {  // 空间足够
            // 写入数据
            output_file.write(reinterpret_cast<const char*>(&line), sizeof(Data_line));
            ++total_lines;  // 行总数增加
            ++one_page_line_num;
        } else {
            std::streampos new_index = get_new_page(&now_header, &now_ender);  // 获取到新页的起始偏移量

            output_file.seekp(head_index);  // 跳转到开头
            now_header.num_lines = total_lines;  // 更新总数
            output_file.write(reinterpret_cast<const char*>(&now_header), sizeof(Header));  //写入磁盘

            total_lines = 0;  // 重置
            one_page_line_num = 0;
            head_index = new_index;

            output_file.seekp(new_index);  // 跳转到新页，继续重新开始读写
        }
    }

    // Update the num_lines field in the header
    now_header.num_lines = total_lines;
    output_file.seekp(head_index);
    output_file.write(reinterpret_cast<const char*>(&now_header), sizeof(Header));  // 重新刷入头部

    input_file.close();
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
//bool buffer_pool(struct Page){
//
//}

