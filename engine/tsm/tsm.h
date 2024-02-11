#ifndef DTIMEDB_TSM_H
#define DTIMEDB_TSM_H

#include <snappy.h>
#include <engine/tsm/tsm_ingredient.h>

namespace dt::tsm
{
    class TSM {
    public:
        TSM() : m_file_manager(20) {}

        // 写入读取header
        bool write_header_to_file(const Header & header, const string & file_path);
        bool read_header_from_file(Header & header, const string & file_path);

        // 写入读取data block
        int64_t write_data_to_file(DataBlock::Type type, int32_t data_num, const string & compress_timestamp, const string & compress_val, const string & file_path, int64_t offset);
        bool read_data_from_file(std::shared_ptr<DataBlock> & data_block, const string & file_path, int64_t offset);

        // 写入读取index entry
        int64_t write_index_entry_to_file(const std::shared_ptr<IndexEntry> & index_entry, const string & file_path, int64_t offset);
        bool read_index_entry_from_file(std::shared_ptr<IndexEntry> & index_entry, const string & file_path, int64_t offset);

        // 写入读取index meta
        bool write_index_meta_to_file(const std::shared_ptr<IndexBlockMeta> & index_meta, const string & file_path, int64_t offset);
        bool read_index_meta_from_file(std::shared_ptr<IndexBlockMeta> & index_meta, const string & file_path, int64_t offset);

        // 写入读取footer
        bool write_footer_to_file(const Footer & footer, const string & file_path);
        bool read_footer_from_file(Footer & footer, const string & file_file);

        // 生成IndexEntry
        std::shared_ptr<IndexEntry> create_index_entry(high_resolution_clock::time_point max_time, high_resolution_clock::time_point min_time, int64_t offset, int32_t size);

        // 生成IndexBlockMeta
        std::shared_ptr<IndexBlockMeta> create_index_meta(DataBlock::Type type, const string & meta_key);

        // 生成完整的TSM File
        bool create_tsm(const Header & header, const Footer & footer, const string & file_path);

        void set_size(uint64_t size) { m_tsm_size = size; }

        // 差值计算 &恢复差值
        std::vector<nanoseconds> calculate_differences(const std::list<high_resolution_clock::time_point> & timestamps);
        std::list<high_resolution_clock::time_point> restore_timestamps(const std::vector<nanoseconds> & differences);
        // 序列化 &反序列化 timestamp
        string serialize_differences(const std::vector<nanoseconds> & differences);
        std::vector<nanoseconds> deserialize_differences(const std::string & serialized_data);
        // 序列化 &反序列化 string
        string serialize_strings(const std::list<string> & strings);
        std::list<string> deserialize_strings(const string & serialized);
        // snappy压缩 &解压
        string compress_data(const string & serialized_data);
        string decompress_data(const string & compressed_data);
        // 计算timestamp & val 需要对空间
        string calculate_timestamp_size(const std::list<high_resolution_clock::time_point> & timestamps);
        string calculate_val_size(const std::list<string> & strings);

    private:
        FileIOManager m_file_manager;
        uint64_t m_tsm_size;  // TSM size
    };
}

#endif //DTIMEDB_TSM_H
