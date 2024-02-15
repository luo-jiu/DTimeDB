#ifndef DTIMEDB_ISYSTEM_H
#define DTIMEDB_ISYSTEM_H

#include <string>
using std::string;

namespace dt::impl
{
    class ISystem
    {
    public:
        /**
        * 查看表系统文件信息
        */
        virtual bool sys_show_file(
                string & db_name,
                string & tb_name) = 0;

        /**
         * 修改系统表信息
         */
        virtual bool sys_update_file(
                string & db_name,
                string & tb_name,
                const string & where) = 0;

        /**
         * 清空系统表信息
         */
        virtual bool sys_clear_file(
                string & db_name,
                string & tb_name) = 0;

    };
};

#endif //DTIMEDB_ISYSTEM_H
