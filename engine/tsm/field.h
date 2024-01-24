#ifndef DTIMEDB_FIELD_H
#define DTIMEDB_FIELD_H

#include <engine/tsm/tsm_ingredient.h>
#include <engine/tsm/skip_list.h>
#include <engine/tsm/tsm.h>

#include <list>
#include <memory>
#include <deque>

namespace dt::tsm
{
    class Field
    {
    public:
        Field() = default;
        Field(DataBlock::Type type, string field_name) : m_type(type), m_field_name(field_name), m_status(false) {}
        string write(high_resolution_clock::time_point timestamp, string & data, string & db_name, string & tb_name);
        bool get_status() const { return m_status; }

        bool should_flush_data();

        bool get_data_status();
        bool get_index_status();
        int get_index_deque_size();
        SkipList<string> & get_skip_list();
        high_resolution_clock::time_point get_skip_list_time_point();  // 获取到跳表的时间戳
        void push_data_to_deque(std::shared_ptr<DataBlock> data_block);
        std::shared_ptr<DataBlock> pop_data_from_deque();
        void push_index_to_deque(std::shared_ptr<IndexEntry> & index_block);
        std::shared_ptr<IndexEntry> pop_index_from_deque();

    public:
        string                                      m_field_name;           // 字段名
        DataBlock::Type                             m_type;                 // 字段数据类型
        high_resolution_clock::time_point           m_index_last_time;      // 索引刷盘计时器

    private:
        SkipList<string>                            m_sl;                   // 跳表

        bool                                        m_status;               // 状态
        high_resolution_clock::time_point           m_sl_last_time;         // 跳表刷块计时器

        std::mutex                                  m_sl_mutex;             // 有关跳表逻辑的锁
        std::mutex                                  m_data_lock;
        std::mutex                                  m_index_lock;

        std::shared_ptr<DataBlock>                  m_current_data;         // 当前块

        std::deque<std::shared_ptr<DataBlock>>      m_data_deque;           // data 队列
        std::deque<std::shared_ptr<IndexEntry>>     m_index_deque;          // index 队列
    };
}

#endif //DTIMEDB_FIELD_H
