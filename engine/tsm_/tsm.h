//#pragma once
//
//#include "tsm_ingredient.h"
//
//using std::string;
//
//namespace dt
//{
//    namespace tsm {
//        class TSM {
//        public:
//            // 写入读取header
//            bool write_header_to_file(const Header & header, const string & file_path);
//            bool read_header_from_file(Header & header, const string & file_path);
//
//            // 写入读取data block
//            template <class T>
//            u_int64_t write_data_to_file(const std::shared_ptr<DataBlock<T>> & data_block, const string & file_path, int64_t offset) const;
//
//            template <class T>
//            bool read_data_from_file(DataBlock<T> & data_block, const string & file_path, int64_t offset);
//
//            // 写入读取index entry
//            u_int64_t write_index_entry_to_file(const std::shared_ptr<IndexEntry> & index_entry, const string & file_path, int64_t offset) const;
//            bool read_index_entry_from_file(IndexEntry & index_entry, const string & file_path, int64_t offset);
//
//            // 写入读取index meta
//            bool write_index_meta_to_file(const std::shared_ptr<IndexBlockMeta> & index_meta, const string & file_path, int64_t offset) const;
//            bool read_index_meta_from_file(std::shared_ptr<IndexBlockMeta> & index_meta, const string & file_path, int64_t offset);
//
//            // 写入读取footer
//            bool write_footer_to_file(const Footer & footer, const string & file_path);
//            bool read_footer_from_file(Footer & footer, const string & file_file);
//
//            // 生成IndexEntry
//            std::shared_ptr<IndexEntry> create_index_entry(high_resolution_clock::time_point max_time, high_resolution_clock::time_point min_time, int64_t offset, int32_t size);
//
//            // 生成IndexBlockMeta
//            std::shared_ptr<IndexBlockMeta> create_index_meta(IndexBlockMeta::Type type, const string & measurement, const string & field);
//        };
//
//        /**
//         * 将数据块写入
//         * @return 返回块大小
//         */
//        template <class T>
//        u_int64_t TSM::write_data_to_file(
//                const std::shared_ptr<DataBlock<T>> & data_block ,
//                const string & file_path,
//                int64_t offset) const
//        {
//            auto & file = FileManager::get_output_stream(file_path);
//            if (!file.is_open())
//            {
//                std::cerr << "Error: Could not open file for writing - from engine/tsm_/tsm_.h" << std::endl;
//                return -1;
//            }
//            file.seekp(offset);  // 移动到指定位置
//
//            u_int64_t size = 8;
//            long timestamp_size = sizeof(high_resolution_clock::time_point);
//
//            // 将类型 长度写入文件
//            file.write(reinterpret_cast<const char*>(&data_block->m_type), sizeof(data_block->m_type));
//            file.write(reinterpret_cast<const char*>(&data_block->m_length), sizeof(data_block->m_length));
//
//            // 将时间戳写入文件
//            for (const auto & timestamp : data_block->m_timestamps)
//            {
//                file.write(reinterpret_cast<const char*>(&timestamp), sizeof(timestamp));
//            }
//            size += timestamp_size * data_block->m_timestamps.size();
//
//            // 将数据写入文件
//            for (const auto & value : data_block->m_values)
//            {
//                file.write(reinterpret_cast<const char*>(&value), sizeof(value));
//            }
//            size += 4 * data_block->m_values.size();
//
//            file.flush();
//            return size;
//        }
//
//        template <class T>
//        bool TSM::read_data_from_file(
//                DataBlock<T> & data_block,
//                const string & file_path,
//                int64_t offset)
//        {
//            auto & file = FileManager::get_input_stream(file_path);
//            if (!file.is_open())
//            {
//                std::cerr << "Error: Could not open file for reading - from engine/tsm_/tsm_.h" << std::endl;
//                return false;
//            }
//            file.seekg(offset);
//
//            // 读取类型 长度
//            file.read(reinterpret_cast<char*>(&data_block.m_type), sizeof(data_block.m_type));
//            file.read(reinterpret_cast<char*>(&data_block.m_length), sizeof(data_block.m_length));
//
//            // 读取时间戳
//            data_block.m_timestamps.clear();
//            auto num = data_block.m_length;
//            for (size_t i = 0; i < num; ++i)
//            {
//                high_resolution_clock::time_point timestamp;
//                file.read(reinterpret_cast<char*>(&timestamp), sizeof(timestamp));
//                data_block.m_timestamps.push_back(timestamp);
//            }
//
//            // 读取值
//            data_block.m_values.clear();
//            for (size_t i = 0; i < num; ++i)
//            {
//                T value;
//                file.read(reinterpret_cast<char*>(&value), sizeof(value));
//                data_block.m_values.push_back(value);
//            }
//
//            return true;
//        }
//    }
//}
//
//
//
//
//
//
//
//
