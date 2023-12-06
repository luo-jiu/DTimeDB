//
// Created by illumwang on 12/5/23.
//
#pragma once
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
#include <json/json.h>
using namespace std::chrono;
using namespace std;
using namespace luo::json;

const uint32_t PAGE_SIZE=4096;
namespace circular_list
{
    //行头，包括行id，页名,以及下一行的位置
    class Meta
    {
    private:
        uint32_t        m_id;
        char            *m_page_name;
        Meta            *m_next_row;
    public:

        Meta(){}
        Meta(uint32_t new_id,const char *new_page_name)
        {
            m_id=new_id;
            m_page_name=new char [strlen(new_page_name)+1];
            m_next_row= nullptr;
        }
        ~Meta()
        {
            free(m_page_name);
        }
    };
    class Row
    {
    private:
        Meta *meta;
        Timestamp timestamp;
        DATA_TYPE type;
        union
        {
            int         intValue;
            double      doubleValue;
            char        charValue;
            string*     stringValue;
        };
        Row* next_row;
    public:
        Row(uint32_t meta_id, const char *page_name, Timestamp _time, DATA_TYPE _type): meta(new Meta(meta_id, page_name)), timestamp(_time), type(_type), next_row(nullptr){
            switch (type) {
                case DATA_TYPE::INTEGER:
                    intValue = 0; //整数类型
                    break;
                case DATA_TYPE::DOUBLE:
                    doubleValue = 0.0; //浮点数类型
                    break;
                case DATA_TYPE::CHAR:
                    charValue = '\0'; //字符类型
                    break;
                case DATA_TYPE::STRING:
                    stringValue = new std::string(); //动态分配内存
                    break;
                default:
                    break;
            }
        }
        ~Row()=default;
        Row(){}
        size_t estimateRowSize();
        void calculate_row_size(Row& row);
        bool drop_from_pages(Row& row);
    };

    class PageHead
    {
    private:
        uint32_t          m_page_id;
        char             *m_block_name;
        char             *m_page_name;
        Page_TYPE         m_type;
        PageHead         *m_next_page;
    public:
        PageHead(){}
        PageHead(uint32_t _page_id,char *_block_name, char *_page_name, Page_TYPE _type): m_type(_type), m_next_page(nullptr){
            m_block_name= strdup(_block_name);
            m_page_name= strdup(_page_name);
        }
        ~PageHead()=default;
    };
    class PageTail
    {
    private:
        int32_t m_offset;
    public:
        PageTail(int32_t _offset): m_offset(_offset){}
        ~PageTail()=default;
    };

    class Page
    {
    private:
        PageHead            *m_page_head;
        vector<Row>          m_rows;
        PageTail            *m_page_tail;
    public:
        Page(){}
        Page(PageHead *pageHead,PageTail *pageTail): m_page_head(pageHead), m_page_tail(pageTail){}
        ~Page()=default;
        bool insert_row(const Row& new_row);
        bool drop_row(int *row_id);
        void select_row();
    };


    class  IndexPageHeader{

    };
    class IndexPage{

    };
}
#endif //DTIMEDB_CIRCULARLIST_STRUCT_H
