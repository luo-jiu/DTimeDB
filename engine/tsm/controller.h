#ifndef DTIMEDB_CONTROLLER_H
#define DTIMEDB_CONTROLLER_H

#include <file_manager/file_path_manager.h>
using namespace dt::file;

#include <engine/iengine/iengine.h>
using namespace dt::iengine;

#include <engine/tsm/write.h>
using namespace dt::tsm;

namespace dt::tsm
{
    /**
     * 控制层
     *
     * 管理所有数据库
     */
    class Controller : public IEngine
    {
    public:
        void init();

        bool create_database(string & db_name) override;
        bool use_database(string & db_name) override;
        bool create_table(string & tb_name, string & db_name) override;

        bool insert(high_resolution_clock::time_point timestamp, string value, Type type, string & field_name, string & measurement, string & db_name) override;
        bool update(high_resolution_clock::time_point timestamp, string value, Type type, string & table_name) override;

        bool get_next_data(string & data) override;
        void begin_indexed_scan(const high_resolution_clock::time_point & timestamp, string & data) override;
        bool get_range_datas(const high_resolution_clock::time_point & start, const high_resolution_clock::time_point & end, std::vector<string> & datas) override;
        bool get_range_datas(std::vector<string> tags, std::vector<string> datas) override;

    private:
        struct Table
        {
            // 一个write 负责一个表的全部写入
            std::shared_ptr<Write> m_writer;
        };

        struct Database
        {
            string                       m_name;
            std::map<string, Table>      m_table_map;
        };

        //       db_name
        std::map<string, Database>      m_map;
        FilePathManager                 m_file;  // 文件管理器
    };
}

#endif //DTIMEDB_CONTROLLER_H
