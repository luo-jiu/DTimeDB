#ifndef DTIMEDB_SHARD_H
#define DTIMEDB_SHARD_H

#include "wal/wal.h"

#include <iostream>
#include <chrono>
#include <ctime>
#include <vector>

namespace dt::tsm
{
    /**
     * 数据分片
     */
class Shard
    {
    public:
        Shard(const std::string & databases, const std::string & measurement, const std::string & shard_id);

        std::pair<std::time_t, std::time_t> get_day_range(std::time_t timestamp, int days = 1);
        static std::string timestamp_to_shard_key(std::time_t timestamp);
        static std::time_t shard_key_to_timestamp(const std::string & shard_id);

        std::string serialize();
        void deserialize();

        void update_file_point_offset(const std::string & point_type, int64_t offset);
        void update_file_margin(int64_t margin);
        void create_tsm_file(const std::string & new_tsm_file_path, int64_t margin);

    public:
        std::string                                 m_retention_policy;  // 保留策略
        std::string                                 m_shard_id;  // 分片键

        std::string                                 m_db_name;
        std::string                                 m_mea_name;
        std::vector<std::string>                    m_tsm_files; // 分片对应的TSM文件列表

        // 当前的tsm 文件的信息
        std::string                                 m_curr_tsm_file_path;
        int64_t                                     m_curr_tsm_margin{};
        int64_t                                     m_curr_tsm_head_offset{};
        int64_t                                     m_curr_tsm_tail_offset{};
    };
}

#endif //DTIMEDB_SHARD_H
