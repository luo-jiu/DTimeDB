#ifndef DTIMEDB_WRITE_H
#define DTIMEDB_WRITE_H

#include "file_manager/file_path_manager.h"
#include "engine/impl/iobserver_shard_state.h"
#include "field.h"
#include "field_state.h"
#include "tsm_ingredient.h"
#include "table_state.h"
#include "wal/wal.h"
#include "proto/Shard.pb.h"

#include <unordered_set>
#include <list>
#include <set>
#include <condition_variable>
#include <unordered_map>
#include <string>
#include <chrono>

#define DATA_BLOCK_MARGIN (512)

namespace dt::tsm
{
    /**
     * 总的写入控制
     *
     * 一个Write 负责一个表
     */
class Write : public dt::impl::IShardStateSubject
    {
    public:
        Write() = default;
        Write(std::string & db_name, std::string & tb_name) : m_tb_name(tb_name), m_db_name(db_name), m_head_offset(8), m_tail_offset(DATA_BLOCK_MARGIN), m_margin(DATA_BLOCK_MARGIN), m_is_ready(0){}

        void init();
        void set_file_path_manager(dt::file::FilePathManager * file_path_manager);

        void write(std::chrono::high_resolution_clock::time_point timestamp, std::string & series_key, std::string & data, DataBlock::Type type, std::string & field_name, std::string & db_name, std::string & tb_name, FieldState & tb_state, TableState & queue_state);
        void queue_flush_disk();

        bool should_flush_index(const std::string & field_name);
        void push_back_field_list(const std::string & field);
        std::string pop_front_field_list();
        bool get_data_status();
        void push_data_to_deque(std::shared_ptr<DataBlock> & data_block);
        std::shared_ptr<DataBlock> pop_data_from_deque();
        void push_index_to_deque(const std::string & shard_id, const std::string & field_name, const std::shared_ptr<IndexEntry> & index_block);
        std::shared_ptr<IndexEntry> pop_index_from_deque(const std::string & field_name);
        size_t get_index_deque_size(const std::string & field_name);
        bool task_queue_empty();
        void push_task(const std::string & field_name);
        std::string pop_task();

        bool empty_field_list();
        int size_field_list();

        bool skip_need_flush_data_block(const std::string & shard_id, const std::string & field_name);
        bool index_need_flush_data_block(const std::string & field_name);

        void shard_attach(dt::impl::IShardStateObserver * observer) override;
        void shard_detach(dt::impl::IShardStateObserver * observer) override;
        void shard_notify(bool is_registered) override;

        static std::string timestamp_to_shard_key(std::time_t timestamp);
        static std::time_t shard_key_to_timestamp(const std::string & shard_id);

        std::list<std::string> get_shard_in_meta();

        bool add_shard(const std::string & shard_id, std::unique_ptr<Shard> & shard);
        std::unique_ptr<Shard> & get_shard(const std::string & shard_id);

    private:
        std::shared_ptr<Field> get_field(std::string & field_name, std::string & series_key, const std::string & type, FieldState & tb_state, TableState & queue_state);
        bool fields_empty();

    private:
        std::string                                                          m_db_name;          // 数据库名
        std::string                                                          m_tb_name;          // 测量值/表名

        int64_t                                                              m_head_offset;      // 头指针偏移量
        int64_t                                                              m_tail_offset;      // 尾指针偏移量
        uint64_t                                                             m_margin;           // data block 空间余量
        std::string                                                          m_curr_file_path;   // 当前需要刷写的文件

        std::mutex                                                           m_write_mutex;
        std::mutex                                                           m_flush_disk_mutex;
        std::mutex                                                           m_task_deque_mutex;
        std::mutex                                                           m_data_lock;        // 保证m_data_deque 的线程安全
        std::mutex                                                           m_index_lock;       // 保证m_index_deque 的线程安全, 这个锁大概是无用的后期确定后删除
        std::mutex                                                           m_field_list_mutex;
        mutable std::shared_mutex                                            m_filed_map_mutex;  // 针对m_field_map 安全的读写所
        std::condition_variable                                              m_cv;
        int                                                                  m_is_ready;

        dt::file::FilePathManager *                                          m_file_path;        // 依赖注入，文件管理器

        TSM                                                                  m_tsm;
        std::list<std::string>                                               m_field_list;       // 该list 表明哪些字段有块待写入
        std::set<std::string>                                                m_index_set;        // 该set 用于创建新TSM 刷入没处理完的meta 和entry
        std::unordered_map<std::string, std::shared_ptr<Field>>              m_field_map;

        std::deque<std::shared_ptr<DataBlock>>                               m_data_deque;       // 存储所有字段的 data block

        struct IndexEntryInfo
        {
            std::chrono::high_resolution_clock::time_point                   m_last_time;        // 索引计时器
            std::deque<std::shared_ptr<IndexEntry>>                          m_index_deque;
            std::string                                                      m_shard_id;
        };
        std::map<std::string, IndexEntryInfo>                                m_index_map;        // 存储所有字段的 index entry
        std::unordered_set<std::string>                                      m_task_set;
        std::list<std::string>                                               m_index_task_queue; // 使用list 配合set实现去重顺序任务注册

        mutable std::shared_mutex                                            m_mea_shard_mutex;
        //                 shard id
        std::unordered_map<std::string, std::unique_ptr<Shard>>              m_mea_shard_map;    // 存储测量中所有的 shards
        std::shared_ptr<dt::wal::IWAL> m_wal = std::make_shared<dt::wal::WALRecord>();           // 存储wal工具接口

        mutable std::shared_mutex                                            m_observer_mutex;
        std::list<dt::impl::IShardStateObserver*>                            m_observers;        // 观察者列表
    };
}
#endif //DTIMEDB_WRITE_H
