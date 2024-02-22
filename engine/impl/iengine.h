#ifndef DTIMEDB_IENGINE_H
#define DTIMEDB_IENGINE_H

#include "file_manager/file_path_manager.h"

#include <string>
#include <chrono>
#include <list>
#include <vector>

namespace dt::impl
{
    /**
     * [共有部分]
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
         * 1. 加载数据库信息
         * 2. 打印表
         *
         * 因为将每个数据库的文件管理系统给分开了，所以需要额外提供接口
         */
        virtual bool load_database(std::string & db_name, std::vector<std::string> & tables) = 0;
        virtual void show_table(std::string & db_name) = 0;


        /**
         * 创建数据库
         */
        virtual bool create_database(
                std::string & db_name) = 0;

        /**
         * 创建表
         */
        virtual bool create_table(
                std::string & tb_name,
                std::string & db_name) = 0;


        virtual bool update(
                std::chrono::high_resolution_clock::time_point timestamp,
                std::string value,
                Type type,
                std::string & table_name) = 0;


        /**
         * 获取下一条数据 (不使用索引)
         * 最基本的接口，允许逐条数据遍历表中的数据
         *
         * 通常用于[全表扫描] 和没有可用索引时的操作
         */
        virtual bool get_next_data(
                std::string & data) = 0;


        /**
         * 获取准确的数据 (通过索引 [时间戳timestamp])
         * @param timestamp
         */
        virtual void begin_indexed_scan(
                const std::chrono::high_resolution_clock::time_point & timestamp,
                std::string & data) = 0;


        /**
         * 获取范围数据 (通过索引 [时间戳timestamp])
         */
        virtual bool get_range_data(
                const std::chrono::high_resolution_clock::time_point & start,
                const std::chrono::high_resolution_clock::time_point & end,
                std::vector<std::string> & data) = 0;


        /**
         * 全表扫描
         */
        virtual std::list<std::string> scan_full_table(
                const std::string & db_name,
                const std::string & tb_name) = 0;
    };
}



#endif //DTIMEDB_IENGINE_H
