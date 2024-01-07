#ifndef DTIMEDB_CONTROLLER_H
#define DTIMEDB_CONTROLLER_H

#include <file_manager/file_path_manager.h>
using namespace dt::file;

#include <engine/impl/iengine.h>
using namespace dt::impl;

#include <engine/tsm/write.h>
using namespace dt::tsm;

#include <thread_manager/thread_pool.h>
using namespace dt::thread;

#include <engine/tsm/table_state.h>

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
        Controller(): m_thread_pool(8), m_running(true), m_file("tsm")
        {
            init();
        }

        ~Controller()
        {
            stop_monitoring_thread();
            if (m_monitor_thread.joinable())
            {
                m_monitor_thread.join();
            }
        }

        void init();

        void load_database(string & db_name) override;
        void show_table(string & db_name) override;
        bool create_database(string & db_name) override;
        bool create_table(string & tb_name, string & db_name) override;

        bool insert(high_resolution_clock::time_point timestamp, string value, Type type, string & field_name, string & measurement, string & db_name) override;
        bool update(high_resolution_clock::time_point timestamp, string value, Type type, string & table_name) override;

        bool get_next_data(string & data) override;
        void begin_indexed_scan(const high_resolution_clock::time_point & timestamp, string & data) override;
        bool get_range_datas(const high_resolution_clock::time_point & start, const high_resolution_clock::time_point & end, std::vector<string> & datas) override;
        bool get_range_datas(std::vector<string> tags, std::vector<string> datas) override;

        void monitoring_thread();
        void stop_monitoring_thread();

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
        ThreadPool                      m_thread_pool;  // 线程池
        FilePathManager                 m_file;         // 文件管理器(每个引擎都有自己的管理系统)

        TableState                      m_state;        // 为监控线程提供表状态
        std::atomic<bool>               m_running;      // 用于退出监控线程
        std::thread                     m_monitor_thread;
    };
}

#endif //DTIMEDB_CONTROLLER_H
