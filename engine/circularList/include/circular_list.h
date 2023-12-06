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
        std::ofstream       m_file;

    };
    template<class E>
    class Block
    {
    private:
        Page* page;
    public:
        Block(const char *file_name)
        {

        }


    };
}

#endif //DTIMEDB_CIRCULARLIST_H
