//#pragma once
//
//#include <deque>
//#include <memory>
//#include <thread>
//
//#include <engine/tsm_/skip_list.h>
//#include <engine/tsm_/tsm.h>
//
//
//namespace dt
//{
//    namespace tsm
//    {
//        /**
//         * 控制器，控制写入TSM
//         *
//         * 其实就是写一类data 块和index 块
//         * 只针对TSM 中的一个部分
//         */
//        template <class T>
//        class Controller
//        {
//        public:
//            Controller(): m_tsm(), m_entry_count(0), m_exist_index_meta(false), m_data_offset(0), m_index_offset(0) {}
//            Controller(string & file_path): m_file_path(file_path), m_entry_count(0), m_exist_index_meta(false), m_data_offset(0), m_index_offset(0){}
//            ~Controller() = default;
//
//            void set_file_path(string & file_path);
//            void set_measurement(string & measurement);
//            void set_field(string & field);
//
//            void write(high_resolution_clock::time_point timestamp, T value);
//            void write_file();
//            void flush_write_file();
//
//        private:
//            void push_data_to_deque(std::shared_ptr<DataBlock<T>> data_block);
//            std::shared_ptr<DataBlock<T>> pop_data_from_deque();
//            bool is_data_empty();
//            void push_index_to_deque(std::shared_ptr<IndexEntry> & index_block);
//            std::shared_ptr<IndexEntry> pop_index_from_deque();
//            bool is_index_empty();
//
//        private:
//            TSM                                             m_tsm;
//            SkipList<T>                                     m_sl;
//            std::mutex                                      m_data_lock;
//            std::mutex                                      m_index_lock;
//
//            std::shared_ptr<DataBlock<T>>                   m_current_data;
//            bool                                            m_exist_index_meta;  // 判断是否有index_meta
//            size_t                                          m_entry_count;       // 默认10 个
//            int64_t                                         m_data_offset;       // 数据偏移量
//            int64_t                                         m_index_offset;      // 索引偏移量
//            string                                          m_file_path;         // TSM 文件路径
//            string                                          m_measurement;       // 测量值
//            string                                          m_field;             // 字段
//
//            std::deque<std::shared_ptr<DataBlock<T>>>       m_data_deque;
//            std::deque<std::shared_ptr<IndexEntry>>         m_index_deque;
//        };
//
//        /**
//         * 将DataBlock 组织好
//         */
//        template <class T>
//        void Controller<T>::write(high_resolution_clock::time_point timestamp, T value)
//        {
//            if (m_sl.size() >= 10)  // 跳表数量大于 10
//            {
//                // 确保m_current_data 不为空
//                if (!m_current_data)
//                {
//                    m_current_data = std::make_shared<DataBlock<T>>();
//                }
//
//                // 写入DataBlock
//                u_int32_t _size = 0;
//                if (std::is_same<T, string>::value)
//                {
//                    for (auto it = m_sl.begin(); it != m_sl.end(); ++it)  // 迭代器
//                    {
//                        if (it != m_sl.end())  // 校验
//                        {
//                            auto key_value = *it;
//                            auto str = key_value.second;
////                            _size += static_cast<u_int16_t>(str.length());  // 累计大小
//                            m_current_data->write(key_value.first, key_value.second);
//                        }
//                    }
//                    _size += m_sl.size() * sizeof(high_resolution_clock::time_point);
//                }
//                else
//                {
//                    for (auto it = m_sl.begin(); it != m_sl.end(); ++it)  // 迭代器
//                    {
//                        if (it != m_sl.end())  // 校验
//                        {
//                            auto key_value = *it;
//                            m_current_data->write(key_value.first, key_value.second);
//                        }
//                    }
//                    _size += m_sl.size() * (sizeof(high_resolution_clock::time_point) + sizeof(T));
//                }
//
//                m_current_data->m_length = m_sl.size();  // 设置大小
//                m_current_data->m_size = _size;
//
//                auto m_current_entry = m_tsm.create_index_entry(m_sl.max_key(), m_sl.min_key(), 0, m_current_data->m_size);
//                push_data_to_deque(m_current_data);  // 存放在队列中
//                push_index_to_deque(m_current_entry);
//
//                m_current_data = std::make_shared<DataBlock<T>>();  // 重置m_current_data 以便接收新数据
//                m_sl.cle();  // 清空跳表
//            }
//            m_sl.put(timestamp, value);
//        }
//
//        /**
//         * 写入DataBlock 和IndexBlock
//         *
//         * 这个落盘函数会在DataBlock 数量达到10 的时候才会落盘
//         */
//        template <class T>
//        void Controller<T>::write_file()
//        {
//            if (!m_exist_index_meta || m_entry_count > 9)  // 没有index_meta 或者 已满
//            {
//                // 创建index_meta
//                std::shared_ptr<IndexBlockMeta> e;
//                if (std::is_same<T, int>::value)
//                {
//                    e = m_tsm.create_index_meta(IndexBlockMeta::Type::DATA_INTEGER, m_measurement, m_field);
//                }
//                else if (std::is_same<T, string>::value)
//                {
//                    e = m_tsm.create_index_meta(IndexBlockMeta::Type::DATA_STRING, m_measurement, m_field);
//                }
//                else if (std::is_same<T, float>::value)
//                {
//                    e = m_tsm.create_index_meta(IndexBlockMeta::Type::DATA_FLOAT, m_measurement, m_field);
//                }
//                // 写入index_meta
//                m_tsm.write_index_meta_to_file(e, m_file_path, m_index_offset);
//                m_exist_index_meta = true;
//
//                std::shared_ptr<IndexBlockMeta> e1(new IndexBlockMeta());
//                m_tsm.read_index_meta_from_file(e1, m_file_path, m_index_offset);
//            }
//
//            if (is_data_empty() && is_index_empty())  // 都得有数据
//            {
//                // 余量空间大小计算
//                if(true)
//                {
//                    // 刷入data_block 和 index_entry
//                    auto _data_size = m_tsm.write_data_to_file(pop_data_from_deque(), m_file_path, m_data_offset);
//                    auto _index_size = m_tsm.write_index_entry_to_file(pop_index_from_deque(), m_file_path, m_index_offset);
//
//                    // 修改偏移量
//                    m_data_offset += _data_size;
//                    m_index_offset -= _index_size;  // 前移
//                }
//                else  // 开启新的TSM File
//                {
//
//                }
//            }
//        }
//
//        /**
//         * 无论有多少DataBlock 都会进行落盘
//         */
//        template <class T>
//        void Controller<T>::flush_write_file()
//        {
//
//        }
//
//        /**
//         * 放置数据
//         */
//        template <class T>
//        void Controller<T>::push_data_to_deque(std::shared_ptr<DataBlock<T>> data_block)
//        {
//            std::lock_guard<std::mutex> lock(m_data_lock);  // 互斥锁
//            m_data_deque.push_back(data_block);
//        }
//
//        /**
//         * 获取数据
//         */
//        template <class T>
//        std::shared_ptr<DataBlock<T>> Controller<T>::pop_data_from_deque()
//        {
//            std::lock_guard<std::mutex> lock(m_data_lock);
//            if (!m_data_deque.empty())
//            {
//                std::shared_ptr<DataBlock<T>> data_block = m_data_deque.front();
//                m_data_deque.pop_front();
//                return data_block;
//            }
//        }
//
//        template <class T>
//        bool Controller<T>::is_data_empty()
//        {
//            return m_data_deque.empty();
//        }
//
//        template <class T>
//        bool Controller<T>::is_index_empty()
//        {
//            return m_index_deque.empty();
//        }
//
//        template <class T>
//        void Controller<T>::push_index_to_deque(std::shared_ptr<IndexEntry> & index_block)
//        {
//            std::lock_guard<std::mutex> lock(m_index_lock);
//            m_index_deque.push_back(index_block);
//        }
//
//        template <class T>
//        std::shared_ptr<IndexEntry> Controller<T>::pop_index_from_deque()
//        {
//            std::lock_guard<std::mutex> lock(m_index_lock);
//            if (!m_index_deque.empty())
//            {
//                std::shared_ptr<IndexEntry> index_block = m_index_deque.front();
//                m_index_deque.pop_front();
//                return index_block;
//            }
//        }
//
//        template <class T>
//        void Controller<T>::set_file_path(string & file_path)
//        {
//            m_file_path = file_path;
//        }
//
//        template <class T>
//        void Controller<T>::set_measurement(string & measurement)
//        {
//            m_measurement = measurement;
//        }
//
//        template <class T>
//        void Controller<T>::set_field(string & field)
//        {
//            m_field = field;
//        }
//    }
//}