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
        string init_file(string & file_path) override;
        bool insert(high_resolution_clock::time_point timestamp, string value, Type type, string & field_name, string & file_path) override;
        bool update(high_resolution_clock::time_point timestamp, string value, Type type, string & table_name) override;
        std::list<string> search(std::list<high_resolution_clock::time_point> timestamps, string & table_name, std::list<string> tags) override;

    private:
        Write m_write;
    };
}

#endif //DTIMEDB_CONTROLLER_H
