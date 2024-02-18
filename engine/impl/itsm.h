#ifndef DTIMEDB_ITSM_H
#define DTIMEDB_ITSM_H

#include "engine/impl/iengine.h"

namespace dt::impl
{
    struct ExprNode
    {
        std::string                     m_val;
        std::shared_ptr<ExprNode>       m_left;
        std::shared_ptr<ExprNode>       m_right;
    };

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
                std::chrono::high_resolution_clock::time_point timestamp,
                std::string & tags_str,
                std::string value,
                IEngine::Type type,
                std::string & field_name,
                std::string & tb_name,
                std::string & db_name) = 0;


        /**
         * 构建索引
         */
        virtual bool create_index(
                std::string & measurement,
                std::list<std::string> & tags) = 0;

        /**
         * [tsm]
         * 获取范围数据 (通过索引 [标签tag])
         */
        virtual bool get_range_data(
                const std::string & db_name,
                const std::string & measurement,
                std::vector<std::string> field,
                std::shared_ptr<ExprNode> expr_node) = 0;
    };
}

#endif //DTIMEDB_ITSM_H
