#ifndef DTIMEDB_FIELD_H
#define DTIMEDB_FIELD_H

#include <engine/impl/iobserver_table_state.h>
using namespace dt::impl;

#include <engine/tsm/tsm_ingredient.h>
#include <engine/tsm/skip_list.h>
#include <engine/tsm/tsm.h>

#include <functional>
#include <atomic>
#include <list>
#include <memory>
#include <deque>

namespace dt::tsm
{
    class Field : public ITableStateSubject
    {
    public:
        Field() = default;
        Field(DataBlock::Type type, string field_name) : m_type(type), m_field_name(field_name), m_status(false) {}

        using DataBlockCallback = std::function<void(std::shared_ptr<DataBlock>&)>;
        // 设置回调函数
        void set_condition_callback(DataBlockCallback callback)
        {
            // 用于给data block 写入到统一位置
            m_data_block_callback = std::move(callback);
        }

        void write(high_resolution_clock::time_point timestamp, string & data, string & db_name, string & tb_name);
        bool get_status() const { return m_status; }

        bool should_flush_data();

//        bool get_data_status();
        bool get_index_status();
        int get_index_deque_size();
        SkipList<string> & get_skip_list();
        bool skip_need_flush_data_block();  // 判断是否需要刷盘
        bool index_need_flush_disk();  // 判读是不是需要将index 组织刷盘
        bool get_mate_status();
        void set_mate_status(bool state);

        void push_data_to_deque(std::shared_ptr<DataBlock> data_block);
//        std::shared_ptr<DataBlock> pop_data_from_deque();
        void push_index_to_deque(std::shared_ptr<IndexEntry> & index_block);
        std::shared_ptr<IndexEntry> pop_index_from_deque();

        void attach(ITableStateObserver * observer) override;  // 添加观察者
        void detach(ITableStateObserver * observer) override;  // 移除观察者
        void notify(const string & db_name, const string & tb_name, const string & field_name, bool is_registered, bool use_index_entry_map) override;  // 通知所有观察者状态发生变化

    public:
        string                                      m_field_name;           // 字段名
        string                                      m_index_block_meta_key; // series_key + field
        DataBlock::Type                             m_type;                 // 类型
        high_resolution_clock::time_point           m_index_last_time;      // 索引刷盘计时器

    private:
        DataBlockCallback                           m_data_block_callback;  // 存储回调函数

        SkipList<string>                            m_sl;                   // 跳表
        std::atomic<bool>                           m_need_reset{true};     // 跳表是否需要重置

        bool                                        m_status;               // 状态
        high_resolution_clock::time_point           m_sl_last_time;         // 跳表刷块计时器

        std::mutex                                  m_sl_mutex;             // 有关跳表逻辑的锁
        std::mutex                                  m_data_lock;
        std::mutex                                  m_index_lock;
        mutable std::shared_mutex                   m_mutex;                // 有关观测者模式的读写锁
        mutable std::shared_mutex                   m_time_mutex;           // 有关时间戳的读写锁

        std::shared_ptr<DataBlock>                  m_current_data;         // 当前块

//        std::deque<std::shared_ptr<DataBlock>>      m_data_deque;           // data 队列
        std::deque<std::shared_ptr<IndexEntry>>     m_index_deque;          // index 队列
        std::atomic<bool>                           m_create_meta{false};   // 是否生成mate
        std::list<ITableStateObserver*>             m_observers;            // 观察者列表
    };
}

#endif //DTIMEDB_FIELD_H
