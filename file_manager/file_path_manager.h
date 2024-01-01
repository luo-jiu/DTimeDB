#ifndef DTIMEDB_FILE_PATH_MANAGER_H
#define DTIMEDB_FILE_PATH_MANAGER_H

#include "file_io_manager.h"

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <map>
#include <list>
#include <fstream>
#include <iostream>
using std::string;

#include <chrono>
using namespace std::chrono;

namespace dt::file
{
    /**
     * 为存储引擎提供File 路径管理
     */
    class FilePathManager
    {
    public:
        FilePathManager(): m_io_file(10) {}
//        FilePathManager(const string & base_path) : m_default_base_path(base_path) {}

        string create_database(const string & db_name);
        bool create_table(const string & tb_name, const string & db_name, const string & engine);
        bool exists_table(const string & tb_name, const string & db_name);
        string create_file(const string & tb_name, const string & db_name, const string & engine_abbrev);
        string create_sys_tfile(const string & tb_name, const string & db_name, const string & engine_abbrev);
        string create_sys_dfile(const string & tb_name, const string & db_name, const string & engine_abbrev);

        bool delete_database(const string & db_name);
        bool delete_table(const string & tb_name, const string & db_name);
        bool delete_file(const string & file_name, const string & tb_name, const string & db_name);

        bool load_database(const string & db_name, const string & engine);
        static bool show_data(const string & type);
    private:
        struct TableInfo
        {
            int64_t                 m_counter;  // 计数器
            string                  m_engine;   // 引擎类型
            std::list<string>       m_files;
        };

        static string                                     m_default_base_path;  // 默认基路径
        //       db_name         tb_name | counter and files
        std::map<string, std::map<string, TableInfo>>     m_map;     // 表对应的文件
        FileIOManager                                     m_io_file;
    };
}

#endif //DTIMEDB_FILE_PATH_MANAGER_H
