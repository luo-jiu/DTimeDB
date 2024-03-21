#ifndef DTIMEDB_FILE_PATH_MANAGER_H
#define DTIMEDB_FILE_PATH_MANAGER_H

#include "engine/tsm/system_info.h"
#include "file_io_manager.h"

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <map>
#include <vector>
#include <list>
#include <mutex>
#include <shared_mutex>
#include <shared_mutex>
#include <fstream>
#include <iostream>
#include <chrono>
#include <utility>
#include <set>

namespace dt::file
{
    /**
     * 为存储引擎提供File 路径管理
     */
    class FilePathManager
    {
    public:
        FilePathManager(std::string engine): m_default_base_path("./../dbs"), m_engine(std::move(engine)), m_io_file(10) {}

        std::string create_database(const std::string & db_name);
        bool create_table(const std::string & tb_name, const std::string & db_name);
        std::string create_file(const std::string & tb_name, const std::string & db_name, const std::string & engine_abbrev);
        std::string create_sys_tfile(const std::string & tb_name, const std::string & db_name, const std::string & engine_abbrev);
        std::string create_sys_dfile(const std::string & tb_name, const std::string & db_name, const std::string & engine_abbrev);

        bool delete_database(const std::string & db_name);
        bool delete_table(const std::string & tb_name, const std::string & db_name);
        bool delete_file(const std::string & file_name, const std::string & tb_name, const std::string & db_name);

        bool load_database(const std::string & db_name, std::vector<std::string> & tables);
        bool show_data(const std::string & type);
        bool exists_table(const std::string & db_name, const std::string & tb_name, bool print);

        dt::tsm::SystemInfo load_system_info(const std::string & db_name, const std::string & tb_name);
        bool create_tsm_file_update_sys_info(const std::string & db_name, const std::string & tb_name, const std::string & file_name, uint64_t margin);
        bool update_system_file_name(const std::string & db_name, const std::string & tb_name, const std::string & file_name);
        bool update_system_file_margin(const std::string & db_name, const std::string & tb_name, uint64_t margin);
        bool update_system_file_offset(const std::string & db_name, const std::string & tb_name, const std::string & type, int64_t offset);
        bool sys_show_file(const std::string & db_name, const std::string & tb_name);
        bool sys_clear_file(const std::string & db_name, const std::string & tb_name);

        std::set<std::string> load_mea_fields(const std::string & db_name, const std::string & tb_name);
        void insert_field_to_file(const std::string & db_name, const std::string & tb_name, const std::string & field);

    public:
        mutable std::shared_mutex                                   m_mutex;
        std::mutex                                                  m_table_meta_mutex;

    private:
        struct TableInfo
        {
            std::list<std::string>                                  m_files;
        };

        std::string                                                 m_engine;             // 属于哪个引擎
        std::string                                                 m_default_base_path;  // 默认基路径
        //       db_name         tb_name | counter and files
        std::map<std::string, std::map<std::string, TableInfo>>     m_map;                // 表对应的文件
        std::map<std::string, std::shared_mutex>                    m_table_locks;        // 细粒度锁
        FileIOManager                                               m_io_file;
    };
}

#endif //DTIMEDB_FILE_PATH_MANAGER_H
