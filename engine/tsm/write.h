#ifndef DTIMEDB_WRITE_H
#define DTIMEDB_WRITE_H

#include <engine/tsm/field.h>
#include <engine/tsm/table_state.h>

#include <chrono>
using namespace std::chrono;

#include <unordered_map>
#include <string>
using std::string;

#include <list>
#include <set>
#include <condition_variable>

namespace dt::tsm
{
    /**
     * 总的写入控制
     *
     * 一个Write 负责一个表
     */
    class Write
    {
    public:
        Write() {}
        Write(string & measurement) : m_measurement(measurement), m_head_offset(8), m_tail_offset(4 * 1024 * 1024), m_margin(4 * 1024 * 1024), m_is_ready(0){}

        void write(high_resolution_clock::time_point timestamp, string & data, DataBlock::Type type, string & field_name, string & db_name, string & tb_name, TableState & tb_state);
        void flush_disk();
        void flush_all_sl();

        // field_list (都是线程安全的)
        void push_back_field_list(const string & field);
        string pop_front_field_list();
        bool empty_field_list();
        int size_field_list();

    private:
        void flush_entry_disk(string & field_name, bool is_remove);

        std::shared_ptr<Field> get_field(string & field_name, const string & type, TableState & tb_state);
        bool fields_empty();

    private:
        string                                                  m_measurement;  // 测量值/表名

        int                                                     m_head_offset;  // 头指针偏移量
        int                                                     m_tail_offset;  // 尾指针偏移量
        int                                                     m_margin;       // 空间余量

        std::mutex                                              m_write_mutex;
        std::mutex                                              m_thread_mutex;
        std::mutex                                              m_field_list_mutex;
        std::condition_variable                                 m_cv;
        int                                                     m_is_ready;

        TSM                                                     m_tsm;
        std::list<string>                                       m_field_list;  // 该list 表明哪些字段有块待写入
        std::set<string>                                        m_index_set;   // 该set 用于创建新TSM 刷入没处理完的meta 和entry
        std::unordered_map<string, std::shared_ptr<Field>>      m_field_map;
    };
}
#endif //DTIMEDB_WRITE_H
