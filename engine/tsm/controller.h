#ifndef DTIMEDB_CONTROLLER_H
#define DTIMEDB_CONTROLLER_H

#include "thread_manager/thread_pool.h"
#include "file_manager/file_path_manager.h"
#include "engine/impl/itsm.h"
#include "engine/impl/isystem.h"
#include "engine/impl/iengine.h"
#include "engine/tsm/write.h"
#include "engine/tsm/index.h"
#include "engine/tsm/field_state.h"
#include "engine/tsm/table_state.h"

namespace dt::tsm
{
    /**
     * 控制层
     *
     * 管理所有数据库
     */
    class Controller : public impl::IEngine, public impl::ISystem, public impl::ITSM
    {
    public:
        Controller(): m_producer_thread_pool(8), m_consumer_thread_pool(6), m_running(true), m_file("tsm")
        {
            init();
            // 设置回调函数
            m_field_state.set_skip_condition_callback(
                    [this](const std::string & db_name, const std::string & tb_name, const std::string & field_name) {
                        // skip_list 数据监控 回调处理函数
                        return is_ready_disk_write(db_name, tb_name, field_name);
                    });
            m_field_state.set_index_condition_callback(
                    [this](const std::string & db_name, const std::string & tb_name, const std::string & field_name) {
                        // index queue监控 回调处理函数
                        return is_ready_index_write(db_name, tb_name, field_name);
                    });
            m_table_state.set_condition_callback(
                    [this](const std::string & db_name, const std::string & tb_name) {
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

        bool load_database(std::string & db_name) override;
        void show_table(std::string & db_name) override;
        bool create_database(std::string & db_name) override;
        bool create_table(std::string & tb_name, std::string & db_name) override;

        bool insert(std::chrono::high_resolution_clock::time_point timestamp, std::string & tags_str, std::string value, Type type, std::string & field_name, std::string & tb_name, std::string & db_name) override;
        void insert_thread(std::chrono::high_resolution_clock::time_point timestamp, const std::string & tags_str, std::string value, Type type, std::string & field_name, std::string & tb_name, std::string & db_name);
        bool create_index(std::string & measurement, std::list<std::string> & tags) override;
        void create_index_thread(std::string & measurement, std::list<std::string> & tags);
        bool update(std::chrono::high_resolution_clock::time_point timestamp, std::string value, Type type, std::string & table_name) override;

        bool get_next_data(std::string & data) override;
        void begin_indexed_scan(const std::chrono::high_resolution_clock::time_point & timestamp, std::string & data) override;
        bool get_range_data(const std::chrono::high_resolution_clock::time_point & start, const std::chrono::high_resolution_clock::time_point & end, std::vector<std::string> & data) override;
        bool get_range_data(const std::string & measurement, std::vector<std::string> field, std::vector<std::string> tags, std::shared_ptr<impl::ExprNode> expr_node) override;

        std::list<std::string> scan_full_table(const std::string & db_name, const std::string & tb_name) override;

        bool sys_show_file(std::string & db_name, std::string & tb_name) override;
        bool sys_update_file(std::string & db_name, std::string & tb_name, const std::string & where) override;
        bool sys_clear_file(std::string & db_name, std::string & tb_name) override;

        void monitoring_thread();
        void stop_monitoring_thread();
        void disk_write_thread(const std::string & db_name, const std::string & tb_name);
        bool exists_table(std::string & db_name, std::string & tb_name);

        // 回调函数
        bool is_ready_disk_write(const std::string & db_name, const std::string & tb_name, const std::string & field_name);
        bool is_ready_index_write(const std::string & db_name, const std::string & tb_name, const std::string & field_name);
        bool disk_write(const std::string & db_name, const std::string & tb_name);

    private:
        struct Table
        {
            // 一个write 负责一个表的全部写入
            std::shared_ptr<Write>           m_writer;
        };

        struct Database
        {
            std::string                      m_name;
            std::map<std::string, Table>     m_table_map;
        };

        //       db_name
        std::map<std::string, Database>      m_map;
        thread::ThreadPool                   m_producer_thread_pool;  // 生产者线程池
        thread::ThreadPool                   m_consumer_thread_pool;  // 消费者(刷盘)线程池
        file::FilePathManager                m_file;                  // 文件管理器(每个引擎都有自己的管理系统)

        FieldState                           m_field_state;
        TableState                           m_table_state;
        std::atomic<bool>                    m_running;               // 用于退出监控线程
        std::thread                          m_monitor_thread;
        mutable std::shared_mutex            m_mutex;                 // 读写锁保证m_map 安全

        Index                                m_index;                 // 索引
    };
}

#endif //DTIMEDB_CONTROLLER_H
