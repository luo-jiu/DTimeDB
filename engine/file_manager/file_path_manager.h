#ifndef DTIMEDB_FILE_PATH_MANAGER_H
#define DTIMEDB_FILE_PATH_MANAGER_H

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <map>
#include <vector>
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
        FilePathManager(const string & base_path) : m_default_base_path(base_path), m_file_counter(INT64_MAX) {}

        string create_database(string & table_name, string & database_name);
        string create_file(string & table_name, string & database_name, string & engine_abbrev);
        bool delete_file(const string & file_path);

    private:
        string                                       m_default_base_path;  // 默认基路径
        std::map<string, std::vector<string>>        m_database_map;       // 数据库列表
        std::map<string, int64_t>                    m_table_count_map;    // 表对应计数器
        int64_t                                      m_file_counter;       // 计数器
    };
}

#endif //DTIMEDB_FILE_PATH_MANAGER_H
