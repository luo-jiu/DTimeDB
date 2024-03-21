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

    };
}



#endif //DTIMEDB_IENGINE_H
