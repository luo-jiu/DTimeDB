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
#include "engine/tsm/iterator.h"

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
        Controller();
        ~Controller();

        void init();

        bool load_database(std::string & db_name, std::vector<std::string> & tables) override;
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
        bool get_range_data(const std::string & db_name, const std::string & measurement, std::vector<std::string> field, std::shared_ptr<impl::ExprNode> expr_node) override;

        // 解析表达式树
        void analytic_expr_tree(const std::string & db_name, const std::string & measurement, std::vector<std::string> field, const std::shared_ptr<impl::ExprNode>& expr_node);
        std::shared_ptr<impl::ExprNode> rebuild_tree_without_tags(const std::string & measurement, const std::shared_ptr<impl::ExprNode> & expr_node, std::vector<std::pair<std::string, std::string>> & tags);
        bool is_tag_comparison(const std::string & measurement, const std::shared_ptr<impl::ExprNode> & node);

        std::list<std::string> scan_full_table(const std::string & db_name, const std::string & tb_name) override;

        bool sys_show_file(std::string & db_name, std::string & tb_name) override;
        bool sys_update_file(std::string & db_name, std::string & tb_name, const std::string & where) override;
        bool sys_clear_file(std::string & db_name, std::string & tb_name) override;

        void monitoring_thread();
        void stop_monitoring_thread();
        void disk_write_thread(const std::string & db_name, const std::string & tb_name);
        bool exists_table(std::string & db_name, std::string & tb_name);

        void sys_load_mea_fields();
        void add_field(const std::string & db_name, const std::string & tb_name, const std::string & field);
        bool mea_field_exist(const std::string & db_name, const std::string & tb_name, const std::string & field);

        // 回调函数
        bool is_ready_disk_write(const std::string & db_name, const std::string & tb_name, const std::string & field_name);
        bool is_ready_index_write(const std::string & db_name, const std::string & tb_name, const std::string & field_name);
        bool disk_write(const std::string & db_name, const std::string & tb_name);

    private:
        struct Table
        {
            // 一个write 负责一个表的全部写入
            std::shared_ptr<Write>                                  m_writer;
        };

        struct Database
        {
            std::string                                             m_name;
            std::map<std::string, Table>                            m_table_map;
        };

        //       db_name
        std::map<std::string, Database>                             m_map;
        thread::ThreadPool                                          m_producer_thread_pool;  // 生产者线程池
        thread::ThreadPool                                          m_consumer_thread_pool;  // 消费者(刷盘)线程池
        file::FilePathManager                                       m_file;                  // 文件管理器(每个引擎都有自己的管理系统)

        FieldState                                                  m_field_state;
        TableState                                                  m_table_state;
        std::atomic<bool>                                           m_running;               // 用于退出监控线程
        std::thread                                                 m_monitor_thread;
        mutable std::shared_mutex                                   m_mutex;                 // 读写锁保证m_map 安全
        mutable std::shared_mutex                                   m_fields_map_mutex;      // 保证m_databases_mea

        Index                                                       m_index;                 // 倒排索引
        struct Measurement
        {
            std::unordered_map<std::string, std::set<std::string>>  m_mea_fields;
        };
        std::unordered_map<std::string, Measurement>                m_db_mea_map;
    };
}

#endif //DTIMEDB_CONTROLLER_H
