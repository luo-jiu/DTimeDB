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
    //表结构，列
    struct Column{
        string          name;
        DATA_TYPE       type;
        Column(const string &_name,const DATA_TYPE &_type):name(_name),type(_type){};
    };
    class Row
    {
    private:
        Meta                    meta;
        Timestamp               timestamp;
        vector<string>          values;             //列值
        Row*                    next_row;
    public:
        Row(Meta &_meta,Timestamp &_timestamp):meta(_meta),timestamp(_timestamp),next_row(nullptr){}
        //构造行数据
        void add_field(DATA_TYPE type,const char *value);
        //计算行大小
        size_t calculate_row_size()const;
    };

    class PageHead
    {
    public:

    private:
        uint32_t          m_page_id;
        string            m_page_name;
        uint16_t          m_page_size;
        Page_TYPE         m_type;
        PageHead         *m_next_page;

    };
    class PageTail
    {
    private:
        uint32_t           m_offset;
    public:
        PageTail(int32_t _offset): m_offset(_offset){}
        ~PageTail()=default;
    };

    struct Page
    {
        static const size_t         PAGESIZE=4096;
        PageHead                    m_page_head;
        vector<Row>                 m_rows;
        PageTail                    m_page_tail;

        uint32_t calculateChecksum() const;
        bool verifyCompleteness()const;
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
