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
class Shard : public dt::wal::WALRecord
    {
    public:
        Shard(const std::string & databases, const std::string & measurement, std::time_t timestamp);

        std::pair<std::time_t, std::time_t> get_day_range(std::time_t timestamp, int days = 1);
        std::string timestamp_to_shard_key(std::time_t timestamp);

        std::string serialize() override;
        void deserialize() override;

    public:
        std::pair<std::time_t, std::time_t>         m_time_range;
        std::string                                 m_db_name;
        std::string                                 m_mea_name;
        std::vector<std::string>                    m_tsm_files; // 分片对应的TSM文件列表
    };
}

#endif //DTIMEDB_SHARD_H
