#ifndef DTIMEDB_IENGINE_H
#define DTIMEDB_IENGINE_H

#include <file_manager/file_path_manager.h>
using namespace dt::file;

#include <iostream>
using std::string;

#include <chrono>
using namespace std::chrono;

#include <list>
#include <vector>

namespace dt::impl
{
    /**
     * 引擎接口定义
     */
    class IEngine
    {
    public:
        enum Type
        {
            DATA_STRING,
            DATA_INTEGER,
            DATA_FLOAT
        };

    public:

        /**
         * [共有]
         * 1. 加载数据库信息
         * 2. 打印表
         *
         * 因为将每个数据库的文件管理系统给分开了，所以需要额外提供接口
         */
        virtual void load_database(string & db_name) = 0;
        virtual void show_table(string & db_name) = 0;


        /**
         * [共有]
         * 创建数据库
         */
        virtual bool create_database(
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
                string & tags_str,
                string value,
                Type type,
                string & field_name,
                string & tb_name,
                string & db_name) = 0;

        /**
         * [tsm]
         *
         * 构建索引
         */
        virtual bool create_index(
                string & measurement,
                std::list<string> & tags) = 0;


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
        virtual bool get_range_data(
                const high_resolution_clock::time_point & start,
                const high_resolution_clock::time_point & end,
                std::vector<string> & data) = 0;


        /**
         * [tsm]
         * 获取范围数据 (通过索引 [标签tag])
         */
        virtual bool get_range_data(
                std::vector<string> tags,
                std::vector<string> data) = 0;

//        /**
//         * 获取对应引擎的文件管理器
//         */
//        virtual FilePathManager get_file_manager() = 0;

        /**
         * [共有]
         * 全表扫描
         */
        virtual std::list<string> scan_full_table(
                const string & db_name,
                const string & tb_name) = 0;
    };
}



#endif //DTIMEDB_IENGINE_H
