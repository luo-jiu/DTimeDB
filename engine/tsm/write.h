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
        void write(high_resolution_clock::time_point, string & data, const string & file_path, DataBlock::Type type);
        void flush_disk();

    private:
        std::shared_ptr<Field> get_field(const string & type);

    private:
        std::unordered_map<string, std::shared_ptr<Field>> m_files;
    };
}
#endif //DTIMEDB_WRITE_H
