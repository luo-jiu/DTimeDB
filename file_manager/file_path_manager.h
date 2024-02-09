#ifndef DTIMEDB_FILE_PATH_MANAGER_H
#define DTIMEDB_FILE_PATH_MANAGER_H

#include "file_io_manager.h"

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <map>
#include <list>
#include <shared_mutex>
#include <fstream>
#include <iostream>
using std::string;

#include <chrono>
#include <utility>
using namespace std::chrono;

namespace dt::file
{
    /**
     * 为存储引擎提供File 路径管理
     */
    class FilePathManager
    {
    public:
        FilePathManager(string engine): m_default_base_path("./../dbs"), m_engine(std::move(engine)), m_io_file(10) {}

        string create_database(const string & db_name);
        bool create_table(const string & tb_name, const string & db_name, const string & engine);
        string create_file(const string & tb_name, const string & db_name, const string & engine_abbrev);
        string create_sys_tfile(const string & tb_name, const string & db_name, const string & engine_abbrev);
        string create_sys_dfile(const string & tb_name, const string & db_name, const string & engine_abbrev);

        bool delete_database(const string & db_name);
        bool delete_table(const string & tb_name, const string & db_name);
        bool delete_file(const string & file_name, const string & tb_name, const string & db_name);

        string get_engine_type(const string & db_name, const string & tb_name);

        bool load_database(const string & db_name);
        bool show_data(const string & type);
        bool exists_table(const string & db_name, const string & tb_name, bool print);

        std::tuple<int64_t, uint64_t, string> load_system_info(const string & db_name, const string & tb_name);
        bool create_tsm_file_update_sys_info(const string & db_name, const string & tb_name, const string & file_name, uint64_t margin);
        bool update_system_file_name(const string & db_name, const string & tb_name, const string & file_name);
        bool update_system_file_margin(const string & db_name, const string & tb_name, uint64_t margin);
        bool update_system_file_offset(const string & db_name, const string & tb_name, int64_t offset);
        bool sys_show_file(const string & db_name, const string & tb_name);
        bool sys_clear_file(const string & db_name, const string & tb_name);

    public:
        mutable std::shared_mutex m_mutex;

    private:
        struct TableInfo
        {
            std::list<string> m_files;
        };

        string                                            m_engine;             // 属于哪个引擎
        string                                            m_default_base_path;  // 默认基路径
        //       db_name         tb_name | counter and files
        std::map<string, std::map<string, TableInfo>>     m_map;                // 表对应的文件
        std::map<std::string, std::shared_mutex>          m_table_locks;        // 细粒度锁
        FileIOManager                                     m_io_file;
    };
}

#endif //DTIMEDB_FILE_PATH_MANAGER_H
