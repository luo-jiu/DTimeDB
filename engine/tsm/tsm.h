#ifndef DTIMEDB_TSM_H
#define DTIMEDB_TSM_H

#include "engine/tsm/tsm_ingredient.h"
#include "file_manager/file_io_manager.h"
#include "proto/Shard.pb.h"

#include <snappy.h>
#include <deque>
#include <shared_mutex>

namespace dt::tsm
{
    class TSM {
    public:
        TSM() : m_file_manager(20) {}

        // 写入读取header
        bool write_header_to_file(const Header & header, const std::string & file_path);
        bool read_header_from_file(Header & header, const std::string & file_path);

        // 写入读取data block
        int64_t write_data_to_file(DataBlock::Type type, int32_t data_num, const std::string & compress_timestamp, const std::string & compress_val, const std::string & file_path, int64_t offset);
        bool read_data_from_file(std::shared_ptr<DataBlock> & data_block, const std::string & file_path, int64_t offset);

        // 写入读取index entry
        int64_t write_index_entry_to_file(const std::shared_ptr<IndexEntry> & index_entry, const std::string & file_path, int64_t offset);
        bool read_index_entry_from_file(std::shared_ptr<IndexEntry> & index_entry, const std::string & file_path, int64_t offset);

        // 写入读取index meta
        int64_t write_index_meta_to_file(const std::shared_ptr<IndexBlockMeta> & index_meta, const std::string & file_path, int64_t offset);
        bool read_index_meta_from_file(std::shared_ptr<IndexBlockMeta> & index_meta, const std::string & file_path, int64_t offset);

        // 写入读取footer
        bool write_footer_to_file(const Footer & footer, const std::string & file_path, int64_t tail_offset);
        bool read_footer_from_file(Footer & footer, const std::string & file_file);

        // 写入Series index block
        bool write_series_index_block_to_file(std::deque<std::shared_ptr<IndexEntry>> & index_entry, std::shared_ptr<IndexBlockMeta> & meta, const std::string & file_path, int64_t tail_offset);

        // 生成IndexEntry
        std::shared_ptr<IndexEntry> create_index_entry(std::chrono::high_resolution_clock::time_point max_time, std::chrono::high_resolution_clock::time_point min_time, int64_t offset, int32_t size);

        // 生成IndexBlockMeta
        std::shared_ptr<IndexBlockMeta> create_index_meta(DataBlock::Type type, const std::string & meta_key);

        // 生成完整的TSM File
        bool create_tsm(const Header & header, const Footer & footer, const std::string & file_path);

        // 差值计算 &恢复差值
        static std::vector<std::chrono::nanoseconds> calculate_differences(const std::list<std::chrono::high_resolution_clock::time_point> & timestamps);
        static std::list<std::chrono::high_resolution_clock::time_point> restore_timestamps(const std::vector<std::chrono::nanoseconds> & differences);
        static std::vector<std::chrono::high_resolution_clock::time_point> restore_timestamps_vector(const std::vector<std::chrono::nanoseconds> & differences);
        // 序列化 &反序列化 timestamp
        static std::string serialize_differences(const std::vector<std::chrono::nanoseconds> & differences);
        static std::vector<std::chrono::nanoseconds> deserialize_differences(const std::string & serialized_data);
        // 序列化 &反序列化 string
        static std::string serialize_strings(const std::list<std::string> & strings);
        static std::list<std::string> deserialize_strings(const std::string & serialized);
        static std::vector<std::string> deserialize_strings_vector(const std::string & serialized);
        // snappy压缩 &解压
        static std::string compress_data(const std::string & serialized_data);
        static std::string decompress_data(const std::string & compressed_data);
        // 计算timestamp &val 需要对空间
        std::string calculate_timestamp_size(const std::list<std::chrono::high_resolution_clock::time_point> & timestamps);
        std::string calculate_val_size(const std::list<std::string> & strings);

    private:
        dt::file::FileIOManager m_file_manager;
    };
}

#endif //DTIMEDB_TSM_H
