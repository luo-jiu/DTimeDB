//
// Created by illumwang on 12/2/23.
//
#pragma
#ifndef DTIMEDB_CIRCULARLIST_H
#define DTIMEDB_CIRCULARLIST_H
#include "circular_list_struct.h"
#include "iostream"
#include "json/json.h"
#include <cstring>
#include <unordered_map>
#include "data_def.h"
#include "vector"
#include <fstream>
using namespace luo::json;
using namespace std;
using namespace luo::json;
using namespace std;
using namespace circular_list;
#define BLOCK_MAX_PAGES 100
namespace circularList
{
    class CircularList
    {
    public:

    };
    //所有页通用结构
    class BlockHeader{
    private:
        uint32_t            m_block_size;


    };
    template<class E>
    class Block
    {
        Block(const char *file_name)
        {
            file.open(file_name,std::ios::out | std::ios::binary);
            if (!file.is_open()){
                std::cerr<<"Error opening file."<<std::endl;
            }
        }
        ~Block(){
            file.close();
        }
        void writePage(const Page& page);

    public:
        std::ofstream        file;
    };
}

#endif //DTIMEDB_CIRCULARLIST_H
