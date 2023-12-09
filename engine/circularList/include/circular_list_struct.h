//
// Created by illumwang on 12/5/23.
//
#pragma once
#ifndef DTIMEDB_CIRCULARLIST_STRUCT_H
#define DTIMEDB_CIRCULARLIST_STRUCT_H
#include "circular_tools.h"

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
        Meta(uint32_t new_id, const char *new_page_name) : m_next_row(nullptr) {
            m_id = new_id;
            m_page_name = new char[strlen(new_page_name) + 1];
            strcpy(m_page_name, new_page_name);
        }
    // Meta 类的析构函数实现
        ~Meta() {
            delete[] m_page_name;
        }
    };
    //行存储的数据 结构，fields
    class Fields
    {
    public:
        DATA_TYPE type;
        union
        {
            int         intValue;
            double      doubleValue;
            char        charValue;
            string*     stringValue;
        };
        Fields():stringValue(nullptr){}
        ~Fields(){
            delete[] stringValue;
        }
    };
    class Row
    {
    private:
        Meta                    meta;
        Timestamp               timestamp;
        vector<Fields>          fields;
        Row*                    next_row;
    public:
        Row(const Meta& _meta,const Timestamp _timestamp):meta(_meta),timestamp(_timestamp),next_row(nullptr){}
        ~Row()=default;
        Row(){}
        size_t estimateRowSize() const;
        void addField(DATA_TYPE type,const char *value);
        void calculate_row_size(Row& row);
    };

    class PageHead
    {
    public:
        PageHead(){}
        PageHead(uint32_t _page_id,char *_block_name, char *_page_name, Page_TYPE _type): m_type(_type), m_next_page(nullptr){
            m_block_name= strdup(_block_name);
            m_page_name= strdup(_page_name);
        }
        ~PageHead()=default;
    private:
        uint32_t          m_page_id;
        char             *m_block_name;
        char             *m_page_name;
        Page_TYPE         m_type;
        PageHead         *m_next_page;

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
        int getSize();
        bool init_page(uint32_t pageId, const char *blockName, const char *pageName, Page_TYPE type);
        bool add_row(const Row& new_row);
        bool drop_row(int *row_id);
        const Row& getRow(size_t index);
        bool createPage();
    };

    //索引页结构
    class  IndexPageHeader{

    };
    class IndexPage{

    };
    //日志页结构
    class LogPageHeader{

    };
    class LogPage{

    };
}
#endif //DTIMEDB_CIRCULARLIST_STRUCT_H
