#ifndef DTIMEDB_TSM_H
#define DTIMEDB_TSM_H

#include <engine/tsm/tsm_ingredient.h>

namespace dt
{
    namespace tsm
    {
        class TSM {
        public:
            // 写入读取header
            bool write_header_to_file(const Header & header, const string & file_path);
            bool read_header_from_file(Header & header, const string & file_path);

            // 写入读取data block
            u_int64_t write_data_to_file(const std::shared_ptr<DataBlock> & data_block, const string & file_path, int64_t offset) const;
            bool read_data_from_file(std::shared_ptr<DataBlock> & data_block, const string & file_path, int64_t offset);

            // 写入读取index entry
            u_int64_t write_index_entry_to_file(const std::shared_ptr<IndexEntry> & index_entry, const string & file_path, int64_t offset) const;
            bool read_index_entry_from_file(std::shared_ptr<IndexEntry> & index_entry, const string & file_path, int64_t offset);

            // 写入读取index meta
            bool write_index_meta_to_file(const std::shared_ptr<IndexBlockMeta> & index_meta, const string & file_path, int64_t offset) const;
            bool read_index_meta_from_file(std::shared_ptr<IndexBlockMeta> & index_meta, const string & file_path, int64_t offset);

            // 写入读取footer
            bool write_footer_to_file(const Footer & footer, const string & file_path);
            bool read_footer_from_file(Footer & footer, const string & file_file);

            // 生成IndexEntry
            std::shared_ptr<IndexEntry> create_index_entry(high_resolution_clock::time_point max_time, high_resolution_clock::time_point min_time, int64_t offset, int32_t size);

            // 生成IndexBlockMeta
            std::shared_ptr<IndexBlockMeta> create_index_meta(IndexBlockMeta::Type type, const string & measurement, const string & field);
        };
    }
}

#endif //DTIMEDB_TSM_H
