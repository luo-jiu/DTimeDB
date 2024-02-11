#ifndef DTIMEDB_WRITE_H
#define DTIMEDB_WRITE_H

#include <engine/tsm/field.h>
#include <engine/tsm/field_state.h>
#include <engine/tsm/tsm_ingredient.h>
#include <engine/tsm/table_state.h>
#include <file_manager/file_path_manager.h>
using namespace dt::file;

#include <unordered_set>
#include <list>
#include <set>
#include <condition_variable>
#include <unordered_map>
#include <string>
using std::string;

#include <chrono>
using namespace std::chrono;

#define DATA_BLOCK_MARGIN (4 * 1024 * 1024)

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
        Write() = default;
        Write(string & db_name, string & tb_name) : m_tb_name(tb_name), m_db_name(db_name), m_head_offset(8), m_tail_offset(DATA_BLOCK_MARGIN), m_margin(DATA_BLOCK_MARGIN), m_is_ready(0){}

        void init();
        void set_file_path_manager(FilePathManager * file_path_manager);

        void write(high_resolution_clock::time_point timestamp, string & series_key, string & data, DataBlock::Type type, string & field_name, string & db_name, string & tb_name, FieldState & tb_state, TableState & queue_state);
        void flush_disk();
        void queue_flush_disk();
        void flush_all_sl();

        void push_back_field_list(const string & field);
        string pop_front_field_list();
        bool get_data_status();
        void push_data_to_deque(std::shared_ptr<DataBlock> & data_block);
        std::shared_ptr<DataBlock> pop_data_from_deque();
        void push_index_to_deque(const string & field_name, const std::shared_ptr<IndexEntry> & index_block);
        std::shared_ptr<IndexEntry> pop_index_from_deque(const string & field_name);
        bool task_queue_empty();
        void push_task(const string & field_name);
        string pop_task();


        bool empty_field_list();
        int size_field_list();

        bool skip_need_flush_data_block(const string & field_name);
        bool index_need_flush_data_block(const string & field_name);
    private:
        void flush_entry_disk(string & field_name, bool is_remove);

        std::shared_ptr<Field> get_field(string & field_name, string & series_key, const string & type, FieldState & tb_state, TableState & queue_state);
        bool fields_empty();

    private:
        string                                                          m_db_name;          // 数据库名
        string                                                          m_tb_name;          // 测量值/表名

        int64_t                                                         m_head_offset;      // 头指针偏移量
        int64_t                                                         m_tail_offset;      // 尾指针偏移量
        uint64_t                                                        m_margin;           // data block 空间余量
        string                                                          m_curr_file_path;   // 当前需要刷写的文件

        std::mutex                                                      m_write_mutex;
        std::mutex                                                      m_task_deque_mutex;
        std::mutex                                                      m_data_lock;        // 保证m_data_deque 的线程安全
        std::mutex                                                      m_index_lock;       // 保证m_index_deque 的线程安全
        std::mutex                                                      m_field_list_mutex;
        mutable std::shared_mutex                                       m_filed_map_mutex;  // 针对m_field_map 安全的读写所
        std::condition_variable                                         m_cv;
        int                                                             m_is_ready;

        FilePathManager *                                               m_file_path;        // 依赖注入，文件管理器

        TSM                                                             m_tsm;
        std::list<string>                                               m_field_list;       // 该list 表明哪些字段有块待写入
        std::set<string>                                                m_index_set;        // 该set 用于创建新TSM 刷入没处理完的meta 和entry
        std::unordered_map<string, std::shared_ptr<Field>>              m_field_map;

        std::deque<std::shared_ptr<DataBlock>>                          m_data_deque;       // 存储所有字段的 data block
        std::map<string, std::deque<std::shared_ptr<IndexEntry>>>       m_index_map;        // 存储所有字段的 index entry
        std::unordered_set<string>                                      m_task_set;
        std::list<string>                                               m_index_task_queue; // 使用list 配合set实现去重顺序任务注册
    };
}
#endif //DTIMEDB_WRITE_H
