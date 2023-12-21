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
#include "filesystem"
using namespace luo::json;
using namespace std;
using namespace luo::json;
using namespace std;
using namespace clt;

namespace clt
{
/**
 * block相当于数据库，table相当于表，行为行信息
 */
     class TableHeader{
     public:
         void set_table_name(string &table_name){}
     private:
         string             table_name;
         string             table_path;
     };
     class Table{

     public:
         Table(string &file_name){}
         void create_column(const string &column_name,DATA_TYPE type);
         void insert_row(const Row &new_row);
         bool  write_to_block();

     private:
         TableHeader                    tableHeader;
     };
     class DataBase{
     public:
         bool  create_table(string &table_name);
         bool  drop_table(string &table_name);
         DataBase(string  file_name)
         {
             table=new Table(file_name);
         }
         void  create_data_base(const string &name);
     public:
         string                     database_name;
         string                     file_path;
         Table                     *table;
     };

    class CircularList
    {
    public:

    private:
        static const size_t                 MAX_PAGES= 8;           //存储最大页数
        PageNode                            *head;                             //存储头结点
        size_t                                  page_count;                     //存储页数
    };
}

#endif //DTIMEDB_CIRCULARLIST_H
