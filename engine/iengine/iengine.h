#ifndef DTIMEDB_IENGINE_H
#define DTIMEDB_IENGINE_H

#include <iostream>
using std::string;

#include <chrono>
using namespace std::chrono;

#include <list>
#include <vector>

namespace dt::iengine
{
    /**
     * 引擎接口定义
     */
    class IEngine
    {
    public:
        enum Type
        {
            DATA_STREAM,
            DATA_INTEGER,
            DATA_FLOAT
        };

    public:
        /**
         * [共有]
         * 创建数据库
         */
        virtual bool create_database(
                string & db_name) = 0;


        /**
         * [共有]
         * 选择数据库
         *
         * 引擎要做的是给对应数据库的信息加载到内存
         * 里面来
         */
        virtual bool use_database(
                string & db_name) = 0;


        /**
         * [tsm]
         * 创建表
         */
        virtual bool create_table(
                string & tb_name,
                string & db_name) = 0;
//
//
//        /**
//         * [clt]
//         * 创建表
//         */
//        virtual bool create_table(
//                std::list<string> field,
//                string tb_name) = 0;
//

        /**
         * [tsm]
         * 写入数据
         *
         * @param type 数据类型
         * @param field_name 字段名
         * @param measurement 表名
         * @param database_name 数据库
         */
        virtual bool insert(
                high_resolution_clock::time_point timestamp,
                string value,
                Type type,
                string & field_name,
                string & measurement,
                string & db_name) = 0;

//        virtual bool insert() = 0;


        virtual bool update(
                high_resolution_clock::time_point timestamp,
                string value,
                Type type,
                string & table_name) = 0;


        /**
         * [共有]
         * 获取下一条数据 (不使用索引)
         * 最基本的接口，允许逐条数据遍历表中的数据
         *
         * 通常用于[全表扫描] 和没有可用索引时的操作
         */
        virtual bool get_next_data(
                string & data) = 0;


        /**
         * [共有]
         * 获取准确的数据 (通过索引 [时间戳timestamp])
         * @param timestamp
         */
        virtual void begin_indexed_scan(
                const high_resolution_clock::time_point & timestamp,
                string & data) = 0;


        /**
         * [共有]
         * 获取范围数据 (通过索引 [时间戳timestamp])
         */
        virtual bool get_range_datas(
                const high_resolution_clock::time_point & start,
                const high_resolution_clock::time_point & end,
                std::vector<string> & datas) = 0;


        /**
         * [tsm]
         * 获取范围数据 (通过索引 [标签tag])
         */
        virtual bool get_range_datas(
                std::vector<string> tags,
                std::vector<string> datas) = 0;

    };
}



#endif //DTIMEDB_IENGINE_H
