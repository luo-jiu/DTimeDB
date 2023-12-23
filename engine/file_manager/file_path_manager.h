#ifndef DTIMEDB_FILE_PATH_MANAGER_H
#define DTIMEDB_FILE_PATH_MANAGER_H

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <map>
#include <list>
#include <iostream>
using std::string;

namespace dt::tsm
{
    /**
     * 为存储引擎提供File 路径管理
     */
    class FilePathManager
    {
    public:
        FilePathManager(const string & base_path) : m_default_base_path(base_path) {}

        string create_database(const string & database_name);
        bool create_table(const string & table_name, const string & database_name);
        string create_file(const string & table_name, const string & database_name, const string & engine_abbrev);

        bool delete_database(const string & database_name);
        bool delete_table(const string & table_name, const string & databases_name);
        bool delete_file(const string & file_name, const string & table_name, const string & database_name);

    private:
        struct TableInfo
        {
            int64_t                 m_counter;
            std::list<string>       m_files;
        };

        string                                            m_default_base_path;  // 默认基路径
        //       db_name         tb_name | counter and files
        std::map<string, std::map<string, TableInfo>>     m_map;     // 表对应的文件
    };
}

#endif //DTIMEDB_FILE_PATH_MANAGER_H
