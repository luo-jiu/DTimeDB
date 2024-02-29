#ifndef DTIMEDB_ITSM_H
#define DTIMEDB_ITSM_H

#include "engine/impl/iengine.h"
#include "engine/tsm/data_point.h"

#include <ctime>
#include <iostream>
#include <iomanip>

namespace dt::impl
{
    struct ExprNode
    {
        std::string                     m_val;
        std::shared_ptr<ExprNode>       m_left;
        std::shared_ptr<ExprNode>       m_right;

        // 判断当前节点是否表示比较操作
        [[nodiscard]] bool is_comparison() const
        {
            return m_val == ">" || m_val == "<" || m_val == "==" || m_val == ">=" || m_val == "<=" || m_val == "!=";
        }

        // 判断当前节点是否表示逻辑操作
        [[nodiscard]] bool is_logical() const
        {
            return m_val == "and" || m_val == "or";
        }

        // 评估方法
        [[nodiscard]] bool evaluate_data(const dt::tsm::FieldDataPoint & dataPoint) const
        {
            // 暂缓实现
        };

        static std::time_t parse_timestamp(const std::string & time_str)
        {
            std::tm tm = {};
            std::istringstream ss(time_str);

            // 匹配 "YYYY-MM-DD HH:MM:SS" 格式
            ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

            // 检查是否成功解析
            if(ss.fail())
            {
                throw std::runtime_error("Failed to parse the timestamp");
            }

            // 将std::tm转换为std::time_t
            // 注意：这里假设tm表示的是本地时间
            std::time_t timestamp = std::mktime(&tm);

            // 检查转换结果
            if (timestamp == -1)
            {
                throw std::runtime_error("Conversion to time_t failed");
            }
            return timestamp;
        }

        [[nodiscard]] bool evaluate_shard(const std::string & shard_id)
        {
            auto shard_start = parse_timestamp(shard_id + " 00:00:00");
            auto shard_end = parse_timestamp(shard_id + " 23:59:59");
            if (m_left->m_val != "time")
            {
                return false;
            }

            // 对于“>”操作，检查条件时间是否小于shard的结束时间，即有部分数据在条件之后
            if (m_val == ">")
            {
                return parse_timestamp(m_right->m_val) < shard_end;
            }
            // 对于“<”操作，检查条件时间是否大于shard的开始时间，即有部分数据在条件之前
            else if (m_val == "<")
            {
                return parse_timestamp(m_right->m_val) > shard_start;
            }
            // “>=”操作，如果条件时间小于或等于shard的结束时间，意味着有数据符合
            else if (m_val == ">=")
            {
                return parse_timestamp(m_right->m_val) <= shard_end;
            }
            // “<=”操作，如果条件时间大于或等于shard的开始时间，意味着有数据符合
            else if (m_val == "<=")
            {
                return parse_timestamp(m_right->m_val) >= shard_start;
            }
            // “==”操作，检查条件时间是否在shard的范围内
            else if (m_val == "==")
            {
                return parse_timestamp(m_right->m_val) >= shard_start && parse_timestamp(m_right->m_val) <= shard_end;
            }
            // “!=”操作，检查条件时间是否不完全等于shard的开始和结束时间
            else if (m_val == "!=")
            {
                return parse_timestamp(m_right->m_val) < shard_start || parse_timestamp(m_right->m_val) > shard_end;
            }
            return false;
        };
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
                std::vector<std::pair<std::string, std::string>> reduce_fields,
                std::shared_ptr<ExprNode> expr_node) = 0;
    };
}

#endif //DTIMEDB_ITSM_H
