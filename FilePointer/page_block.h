//
// Created by luojiu on 23-9-19.
//
#ifndef FILEPOINTER_PAGE_BLOCK_H
#define FILEPOINTER_PAGE_BLOCK_H

#include <iostream>
// 页位置
struct Page_offset {
    char path[16]={};
    std::streampos offset;
};

// 页头信息以及双指针
struct Header {
    char path[16]={};
    uint16_t num_lines;  // number of lines
    struct Page_offset prev;  // prev page
};

// 数据行
struct Data_line {
    uint16_t next_line_offset;  // line offset
    char data[255] = {};
};

struct Ender {
    struct Page_offset next;  // next page
};

std::pair<std::ofstream, std::ifstream> get_new_page(const std::string&, std::streampos);

bool write_page(std::ofstream&&, std::ifstream&&, char* [], int);

void read_page(const std::string&, std::streampos);
#endif //FILEPOINTER_PAGE_BLOCK_H
