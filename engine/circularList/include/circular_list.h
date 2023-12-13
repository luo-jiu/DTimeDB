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
namespace circular_list
{
/**
 * block相当于数据库，页相当于表，行为行信息
 */
    //Block头信息，存储页
    class BlockHeader{
    private:
        string              m_block_name;
        string              m_block_version;
        string              m_block_owner;
    public:
        BlockHeader(){}
    };
    //Block体，存储分页数据，数据在环形链表组织成页后页满存储如Block，Block进行持久化操作
    class Block
    {
    public:
        bool isFull()const;
        //将页写入块内存
        bool write_page_to_file(const PageHead &head,const string &file_path);
        bool getPage();
        bool deletePage(const PageHead& header);
        void storeToFile(const string& file_name);
        bool create_page(const string &page_name);
    public:
        BlockHeader         header;
        vector<Page>        pages;
    };

    //环形链表结构，当每一页存满以后存入块中，块进行落盘存储操作
    struct CircularListNode{
        Page                page;
        CircularListNode    *next;
        CircularListNode(const Page& _page):page(_page),next(nullptr){}
    };
    class CircularList
    {
    public:
        //
        bool createPage(const string &page_name);
        void addRowToCircularList(const Row& row);
        void writePageToBlock(Page *page,Block *block);
        bool isFull();
    private:
        static const size_t     MAX_PAGES= 8;       //存储最大页数
        CircularListNode        *head;              //存储头结点
        size_t                  page_count;         //存储页数
    };
}

#endif //DTIMEDB_CIRCULARLIST_H