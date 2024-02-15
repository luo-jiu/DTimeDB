#ifndef DTIMEDB_ICLT_H
#define DTIMEDB_ICLT_H

#include <list>
#include <string>
using std::string;

namespace dt::impl
{
    class ICLT
    {
        /**
         * 创建表
         */
        virtual bool create_table(
                std::list<string> field,
                string tb_name) = 0;

    };
}

#endif //DTIMEDB_ICLT_H
