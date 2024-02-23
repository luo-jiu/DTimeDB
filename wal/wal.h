#ifndef DTIMEDB_WAL_H
#define DTIMEDB_WAL_H

#include <iostream>
#include <sstream>
#include <vector>
#include <mutex>

namespace dt::wal
{
    class WALRecord
    {
    public:
        virtual std::string serialize() = 0;
        virtual void deserialize() = 0;

    public:
        enum class Type { Create, Update, Delete };
        Type            m_type;
        std::string     m_name;
        std::string     m_data;  // 存储序列化后的数据
    };

    /**
     * 负责写入日志
     */
    class WAL
    {
    public:
        bool add_record(WALRecord & record);

        void flush();

    public:
        std::mutex                  m_wal_mutex;
        std::vector<WALRecord>      m_records;
        size_t                      m_threshold = 100; // 阈值
    };
}

#endif //DTIMEDB_WAL_H
