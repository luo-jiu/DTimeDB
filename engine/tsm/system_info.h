#ifndef DTIMEDB_SYSTEM_INFO_H
#define DTIMEDB_SYSTEM_INFO_H

#include <string>
#include <utility>
using std::string;

namespace dt::tsm
{
    struct SystemInfo
    {
        int64_t     m_head_offset;
        int64_t     m_tail_offset;
        uint64_t    m_margin;
        string      m_curr_file_path;

        SystemInfo() {}
        SystemInfo(int64_t head_offset, int64_t tail_offset, uint64_t margin, string file_name):
            m_head_offset(head_offset), m_tail_offset(tail_offset), m_margin(margin), m_curr_file_path(std::move(file_name)){}
    };
}

#endif //DTIMEDB_SYSTEM_INFO_H
