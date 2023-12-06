#pragma once

#include "tsm_ingredient.h"

using std::string;

namespace dt
{
    namespace tsm {
        class TSM {
        public:
            // 写入读取header
            bool write_header_to_file(const Header & header, const string & file_path);
            bool read_header_from_file(Header & header, const string & file_path);

            // 写入读取data block
            template<class T>
            u_int64_t write_data_to_file(const DataBlock<T> & data_block, const string & file_path, int64_t offset) const;

            template<class T>
            bool read_data_from_file(DataBlock<T> & data_block, const string & file_path, int64_t offset);

            // 写入读取index entry
            bool write_index_entry_to_file(const IndexEntry & index_entry, const string & file_path, int64_t offset) const;
            bool read_index_entry_from_file(IndexEntry & index_entry, const string & file_path, int64_t offset);

            // 写入读取index meta
            bool write_index_meta_to_file(const IndexBlockMeta & index_meta, const string & file_path, int64_t offset) const;
            bool read_index_meta_from_file(IndexBlockMeta & index_meta, const string & file_path, int64_t offset);

            // 写入读取footer
            bool write_footer_to_file(const Footer & footer, const string & file_path);
            bool read_footer_from_file(Footer & footer, const string & file_file);

            // 生成IndexEntry
            IndexEntry * create_index_entry(high_resolution_clock::time_point max_time, high_resolution_clock::time_point min_time, int64_t offset, int32_t size);
        };
    }
}








