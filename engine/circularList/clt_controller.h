#ifndef DTIMEDB_CLT_CONTROLLER_H
#define DTIMEDB_CLT_CONTROLLER_H

#include "engine/impl/iengine.h"
#include "engine/impl/iclt.h"

#include "file_manager/file_path_manager.h"

namespace dt::clt
{
    /**
     * 控制层
     */
    class Controller : public dt::impl::IEngine, public dt::impl::ICLT
    {
    public:
        Controller();

        bool load_database(std::string & db_name, std::vector<std::string> & tables) override;

        bool create_database(std::string & db_name) override;
        bool create_table(std::list<std::pair<std::string, std::string>> & fields, std::string db_name, std::string tb_name) override;
        void show_table(std::string & db_name) override;

    public:
        mutable std::shared_mutex m_database_mutex;

        struct Table
        {
            std::set<std::string> m_fields;
        };

        struct Database
        {
            std::unordered_map<std::string, Table> m_tables;
        };

        dt::file::FilePathManager m_file;

        std::unordered_map<std::string, Database> m_databases;

    };
}

#endif //DTIMEDB_CLT_CONTROLLER_H
