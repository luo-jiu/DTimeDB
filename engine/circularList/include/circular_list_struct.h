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
    public:
        //访问私有字段
        uint32_t  get_id()const{
            return m_id;
        }
        char*  get_page_name()const{
            return m_page_name;
        }
        Meta(){}
        Meta(uint32_t new_id, const char *new_page_name)  {
            m_id = new_id;
            m_page_name = new char[strlen(new_page_name) + 1];
            strcpy(m_page_name, new_page_name);
        }
        //复制构造函数
        Meta(const Meta &meta):m_id(meta.m_id),m_page_name(new char [strlen(meta.m_page_name)+1]){
            strcpy(m_page_name,meta.m_page_name);
        }
        //operator
        Meta &operator=(const Meta &meta){
            if (this != &meta){
                m_id=meta.m_id;
                m_page_name=meta.m_page_name;
            }
            return *this;
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
        Meta                            meta;
        Timestamp                   timestamp;
        vector<Column>          column;            //列名
        vector<string>              values;             //列值
        Row*                           next_row;
    public:
        Row(Meta &_meta,Timestamp &_timestamp): meta(_meta), timestamp(_timestamp), next_row(nullptr){}
        //构造行数据
        void add_field(DATA_TYPE type,const char *value);
        //计算行大小
        size_t calculate_row_size()const;
        void display_row()const;
        ~Row()= default;
    };

    class PageHead
    {
    public:
        PageHead(uint32_t _page_id,const char *_page_name,uint32_t _page_size,PAGE_TYPE _page_type)
        : m_page_id(_page_id), m_page_name(_page_name), m_page_size(_page_size), m_type(_page_type), timestamp() {}
    private:
        uint32_t                           m_page_id;                           //页id
        string                               m_page_name;                      //页名（表名
        uint32_t                           m_page_size;                        //页大小
        PAGE_TYPE                   m_type;                                //页类型
        Timestamp                       timestamp;                            //时间戳

    };
    class PageTail
    {
    private:
        uint32_t           m_offset;
    public:
        PageTail(int32_t _offset): m_offset(_offset){}
        ~PageTail()=default;
        uint32_t calculateOffest() const;
    };

    class Page
    {
        static const size_t             PAGESIZE=4096;
        PageHead                        m_page_head;
        vector<Row>                   m_rows;
        PageTail                          m_page_tail;
    public:
        Page(uint32_t _page_id,const char *_page_name,uint32_t _page_size,PAGE_TYPE _page_type,uint32_t _offset)
        : m_page_head(_page_id,_page_name,_page_size,_page_type),
        m_page_tail(_offset){}
        ~Page()=default;
        size_t calculate_page_size() const;
        //初始化页面
        bool init_page(uint32_t pageId, const char *blockName, const char *pageName, PAGE_TYPE type);
        bool add_row(const Row& new_row);
        bool drop_row(int *row_id);
        const Row& getRow(size_t index);
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
