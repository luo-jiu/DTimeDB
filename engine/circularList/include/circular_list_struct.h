//
// Created by illumwang on 12/5/23.
//
#pragma
#ifndef DTIMEDB_CIRCULARLIST_STRUCT_H
#define DTIMEDB_CIRCULARLIST_STRUCT_H
#include "circular_tools.h"
#define BLOCK_MAX_PAGES 100
#include <chrono>
#include <cstring>
#include <cstdint>
#include <string>
#include <vector>
#include "data_def.h"
using namespace std::chrono;
using namespace std;
const uint32_t PAGE_SIZE=4096;
namespace circular_list
{
    //行头，包括行id，页名,以及下一行的位置
    class Meta
    {
    private:
        uint32_t id;
        char *page_name;
        Meta *next_row;
    public:
        Meta(uint32_t new_id,const char *new_page_name)
        {
            id=new_id;
            page_name=new char [strlen(new_page_name)+1];
            next_row= nullptr;
        }
        ~Meta()
        {
            free(page_name);
        }
    };
    class Row
    {
    private:
        Meta *meta;
        Timestamp timestamp;
        DataType type;
        union
        {
            int intValue;
            double doubleValue;
            char charValue;
            string* stringValue;
        };

    public:
        Row(Meta *_meta,DataType _type):meta(_meta),type(_type){
            switch (type) {
                case DataType::INTEGER:
                    intValue = 0; //整数类型
                    break;
                case DataType::DOUBLE:
                    doubleValue = 0.0; //浮点数类型
                    break;
                case DataType::CHAR:
                    charValue = '\0'; //字符类型
                    break;
                case DataType::STRING:
                    stringValue = new std::string(); //动态分配内存
                    break;
                default:
                    break;
            }
        }
        bool create_new_row();
    };

    class PageHead
    {
    private:
        char *block_name;
        char *page_name;
        PageType type;
        PageHead *next_page;
    public:
        PageHead(){}
        PageHead(char *_block_name,char *_page_name,PageType _type):type(_type),next_page(nullptr){
            block_name= strdup(_block_name);
            page_name= strdup(_page_name);
        }
        ~PageHead()=default;
        void set_page_head();
    };
    class PageTail
    {
    private:
        int32_t offset;
    public:
        PageTail(int32_t _offset):offset(_offset){}
        ~PageTail()=default;
    };

    class Page
    {
    private:
        PageHead *pageHead;
        vector<Row> rows;
        PageTail *pageTail;
    public:
        bool insert_row(const Row& new_row);
        bool drop_row(int *row_id);
        void store_into_block(const Page& _page);

    };

}
#endif //DTIMEDB_CIRCULARLIST_STRUCT_H
