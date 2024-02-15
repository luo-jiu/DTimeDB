#ifndef DTIMEDB_ITSM_H
#define DTIMEDB_ITSM_H

#include <engine/impl/iengine.h>

namespace dt::impl
{
    class ITSM
    {
    public:
        /**
        * [tsm]
        * 写入数据
        *
        * @param type 数据类型
        * @param field_name 字段名
        * @param measurement 表名
        * @param database_name 数据库
        */
        virtual bool insert(
                high_resolution_clock::time_point timestamp,
                string & tags_str,
                string value,
                IEngine::Type type,
                string & field_name,
                string & tb_name,
                string & db_name) = 0;


        /**
         * 构建索引
         */
        virtual bool create_index(
                string & measurement,
                std::list<string> & tags) = 0;


        /**
         * [tsm]
         * 获取范围数据 (通过索引 [标签tag])
         */
        virtual bool get_range_data(
                const string & measurement,
                std::list<string> field,
                std::vector<string> tags,
                std::vector<string> data) = 0;
    };
}

#endif //DTIMEDB_ITSM_H
