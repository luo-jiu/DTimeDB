#ifndef DTIMEDB_CONTROLLER_H
#define DTIMEDB_CONTROLLER_H

#include <engine/iengine/iengine.h>
using namespace dt::iengine;

#include <engine/tsm/write.h>
using namespace dt::tsm;

namespace dt::tsm
{
    /**
     * 控制层
     */
    class Controller : IEngine
    {
    public:
        bool insert(high_resolution_clock::time_point timestamp, string value, Type type, string & field_name) override;
        bool update(high_resolution_clock::time_point timestamp, string value, Type type, string & table_name) override;

        bool get_next_data(string & data) override;
        void begin_indexed_scan(const high_resolution_clock::time_point & timestamp, string & data) override;
        bool get_range_datas(const high_resolution_clock::time_point & start, const high_resolution_clock::time_point & end, std::vector<string> & datas) override;
        bool get_range_datas(std::vector<string> tags, std::vector<string> datas) override;

    private:
        Write m_write;
    };
}

#endif //DTIMEDB_CONTROLLER_H
