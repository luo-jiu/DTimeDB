#ifndef DTIMEDB_FILE_DB_PATH_MANAGER_H
#define DTIMEDB_FILE_DB_PATH_MANAGER_H

#include "file_io_manager.h"

#include <unordered_map>
#include <string>
#include <vector>

namespace dt::file
{
    /**
     * 管理数据库这一层面的文件流
     * 以及用于映射表与引擎
     */
    class TableEngineManager
    {
    public:
        TableEngineManager();

        // 函数指针
        typedef void (TableEngineManager::*show_table_option)(const std::string&, const std::string&);

        std::string create_database(const std::string & db_name);
        std::unordered_map<std::string, std::vector<std::string>> load_database_metadata(const std::string & db_name);
        bool create_table(const std::string & db_name, const std::string & tb_name, std::string & engine);
        bool remove_table(const std::string & db_name, const std::string & tb_name);

        bool database_exist(const std::string & db_name) const;
        void table_engine_type(const std::string & db_name, const std::string & tb_name);
        bool table_engine_is(const std::string & db_name, const std::string & tb_name, const std::string & engine_type);

    public:
        std::mutex                                                                    m_mutex;
        std::string                                                                   m_default_base_path;
        //                 db_name                         tb_name      engine_type
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_tab_eng_map;
        FileIOManager                                                                 m_io_file;
        std::unordered_map<std::string, show_table_option>                            m_option_map;
    };
}

#endif //DTIMEDB_FILE_DB_PATH_MANAGER_H
