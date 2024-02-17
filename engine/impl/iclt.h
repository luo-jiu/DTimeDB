#ifndef DTIMEDB_ICLT_H
#define DTIMEDB_ICLT_H

#include <list>
#include <string>

namespace dt::impl
{
    class ICLT
    {
        /**
         * 创建表
         */
        virtual bool create_table(
                std::list<std::string> field,
                std::string tb_name) = 0;

    };
}

#endif //DTIMEDB_ICLT_H
