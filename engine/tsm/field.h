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
        Field() : m_status(false) {}
        Field(DataBlock::Type type) : m_type(type), m_status(false) {}
        void write(high_resolution_clock::time_point timestamp, string & data);
        bool get_status() const { return m_status; }

    private:
        void push_data_to_deque(std::shared_ptr<DataBlock> data_block);
        std::shared_ptr<DataBlock> pop_data_from_deque();
        void push_index_to_deque(std::shared_ptr<IndexEntry> & index_block);
        std::shared_ptr<IndexEntry> pop_index_from_deque();

    private:
        DataBlock::Type                             m_type;          // 字段数据类型
        SkipList<string>                            m_sl;            // 跳表
        TSM                                         m_tsm;

        bool                                        m_status;        // 状态

        std::mutex                                  m_data_lock;
        std::mutex                                  m_index_lock;

        std::shared_ptr<DataBlock>                  m_current_data;  // 当前块

        std::deque<std::shared_ptr<DataBlock>>      m_data_deque;    // data 队列
        std::deque<std::shared_ptr<IndexEntry>>     m_index_deque;   // index 队列
    };
}

#endif //DTIMEDB_FIELD_H
