#ifndef DTIMEDB_CONTROLLER_H
#define DTIMEDB_CONTROLLER_H

#include <file_manager/file_path_manager.h>
using namespace dt::file;

#include <engine/impl/iengine.h>
using namespace dt::impl;

#include <thread_manager/thread_pool.h>
using namespace dt::thread;

#include <engine/tsm/write.h>
#include <engine/tsm/index.h>

#include <engine/tsm/field_state.h>
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
        Controller(): m_producer_thread_pool(8), m_consumer_thread_pool(6), m_running(true), m_file("tsm")
        {
            init();
            // 设置回调函数
            m_field_state.set_skip_condition_callback(
                    [this](const string & db_name, const string & tb_name, const string & field_name) {
                        // skip_list 数据监控 回调处理函数
                        return is_ready_disk_write(db_name, tb_name, field_name);
                    });
            m_field_state.set_index_condition_callback(
                    [this](const string & db_name, const string & tb_name, const string & field_name) {
                        // index queue监控 回调处理函数
                        return is_ready_index_write(db_name, tb_name, field_name);
                    });
            m_table_state.set_condition_callback(
                    [this](const string & db_name, const string & tb_name) {
                        // data block queue监控 回调处理函数
                        return disk_write(db_name, tb_name);
                    });
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

        bool insert(high_resolution_clock::time_point timestamp, string & tags_str, string value, Type type, string & field_name, string & tb_name, string & db_name) override;
        void insert_thread(high_resolution_clock::time_point timestamp, const string& tags_str, string value, Type type, string & field_name, string & tb_name, string & db_name);
        bool create_index(string & measurement, std::list<string> & tags) override;
        void create_index_thread(string & measurement, std::list<string> & tags);
        bool update(high_resolution_clock::time_point timestamp, string value, Type type, string & table_name) override;

        bool get_next_data(string & data) override;
        void begin_indexed_scan(const high_resolution_clock::time_point & timestamp, string & data) override;
        bool get_range_data(const high_resolution_clock::time_point & start, const high_resolution_clock::time_point & end, std::vector<string> & data) override;
        bool get_range_data(std::vector<string> tags, std::vector<string> data) override;

        std::list<string> scan_full_table(const string & db_name, const string & tb_name) override;

        bool sys_show_file(string & db_name, string & tb_name) override;
        bool sys_update_file(string & db_name, string & tb_name, const string & where) override;
        bool sys_clear_file(string & db_name, string & tb_name) override;

        void monitoring_thread();
        void stop_monitoring_thread();
        void disk_write_thread(const string & db_name, const string & tb_name);
        bool exists_table(string & db_name, string & tb_name);

        // 回调函数
        bool is_ready_disk_write(const string & db_name, const string & tb_name, const string & field_name);
        bool is_ready_index_write(const string & db_name, const string & tb_name, const string & field_name);
        bool disk_write(const string & db_name, const string & tb_name);

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
        ThreadPool                      m_producer_thread_pool;  // 生产者线程池
        ThreadPool                      m_consumer_thread_pool;  // 消费者(刷盘)线程池
        FilePathManager                 m_file;                  // 文件管理器(每个引擎都有自己的管理系统)

        FieldState                      m_field_state;
        TableState                      m_table_state;
        std::atomic<bool>               m_running;               // 用于退出监控线程
        std::thread                     m_monitor_thread;
        mutable std::shared_mutex       m_mutex;                 // 读写锁保证m_map 安全

        Index                           m_index;                 // 索引
    };
}

#endif //DTIMEDB_CONTROLLER_H
