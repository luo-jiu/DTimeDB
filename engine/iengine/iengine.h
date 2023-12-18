#ifndef DTIMEDB_IENGINE_H
#define DTIMEDB_IENGINE_H

#include <iostream>
using std::string;

#include <chrono>
using namespace std::chrono;

#include <list>

namespace dt::iengine
{
    /**
     * 引擎接口定义
     */
    class IEngine
    {
    public:
        enum Type
        {
            DATA_STREAM,
            DATA_INTEGER,
            DATA_FLOAT
        };

    public:
        /**
         *
         */
        virtual string init_file(string & file_path) = 0;
        virtual bool insert(high_resolution_clock::time_point timestamp, string value, Type type, string & field_name, string & file_path) = 0;
        virtual bool update(high_resolution_clock::time_point timestamp, string value, Type type, string & table_name) = 0;
        virtual std::list<string> search(std::list<high_resolution_clock::time_point> timestamps, string & table_name, std::list<string> tags) = 0;
    };
}



#endif //DTIMEDB_IENGINE_H
