#ifndef DTIMEDB_WRITE_H
#define DTIMEDB_WRITE_H

#include <engine/tsm/field.h>

#include <chrono>
using namespace std::chrono;

#include <unordered_map>
#include <string>
using std::string;

namespace dt::tsm
{
    /**
     * 总的控制
     */
    class Write
    {
    public:
        Write() {}
        Write(string & measurement) : m_measurement(measurement), m_head_offset(8), m_tail_offset(4 * 1024 * 1024) {}

        void write(high_resolution_clock::time_point, string & data, const string & file_path, DataBlock::Type type, string & field_name);
        void flush_disk();

    private:
        // 线程安全都加锁
        std::shared_ptr<Field> get_field(string & field_name, const string & type);
        bool fields_empty();

    private:
        string                                                  m_measurement;  // 测量值/表名

        int                                                     m_head_offset;  // 头指针偏移量
        int                                                     m_tail_offset;  // 尾指针偏移量

        std::mutex                                              m_mutex;

        TSM                                                     m_tsm;
        std::unordered_map<string, std::shared_ptr<Field>>      m_fields;
    };
}
#endif //DTIMEDB_WRITE_H
