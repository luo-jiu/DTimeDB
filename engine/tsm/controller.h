#pragma once

#include <memory>
#include <thread>

#include <engine/tsm/skip_list.h>
#include <engine/tsm/tsm.h>
#include <deque>

namespace dt
{
    namespace tsm
    {
        /**
         * 控制器，控制写入TSM
         *
         * 其实就是写一类data 块和index 块
         * 只针对TSM 中的一个部分
         */
        template <class T>
        class Controller
        {
        public:
            Controller(): m_exist_index_meta(false), m_data_offset(0), m_index_offset(0) {}
            Controller(string & file_path): m_file_path(file_path), m_exist_index_meta(false), m_data_offset(0), m_index_offset(0){}
            ~Controller() = default;

            void set_file_path(string & file_path);  // 设置路径

            void write(high_resolution_clock::time_point timestamp, T value);
            void write_data();

        private:
            void push_data_to_deque(DataBlock<T> data_block);
            DataBlock<T> pop_data_from_deque();
            void push_index_to_deque(IndexEntry index_block);
            IndexEntry pop_index_from_deque();

        private:
            TSM                                             m_tsm;
            SkipList<T>                                     m_sl;

            std::unique_ptr<DataBlock<T>>                   m_current_data;

            bool                                            m_exist_index_meta;  // 判断是否有index_meta
            int64_t                                         m_data_offset;  // 数据偏移量
            int64_t                                         m_index_offset;  // 索引偏移量
            string                                          m_file_path;  // TSM 文件路径
            std::mutex                                      m_data_lock;
            std::mutex                                      m_index_lock;
            std::deque<DataBlock<T>>                        m_data_deque;
            std::deque<IndexEntry>                          m_index_deque;
        };

        /**
         * 将数据组织好
         * @tparam T
         * @param timestamp
         * @param value
         */
        template <class T>
        void Controller<T>::write(high_resolution_clock::time_point timestamp, T value)
        {
            if (m_sl.size() >= 10)  // 跳表数量大于 10
            {
                // 确保m_current_data 不为空
                if (!m_current_data)
                {
                    m_current_data = std::make_unique<DataBlock<T>>();
                }

                // 写入DataBlock
                for (auto it = m_sl.begin(); it != m_sl.end(); ++it)  // 迭代器
                {
                    if (it != m_sl.end())  // 校验
                    {
                        auto key_value = *it;
                        m_current_data->write(key_value.first, key_value.second);
                    }
                }
                m_current_data->m_length = m_sl.size();  // 设置大小
                push_data_to_deque(*m_current_data);  // 存放在队列中
                m_current_data = std::make_unique<DataBlock<T>>();  // 重置m_current_data 以便接收新数据
                m_sl.cle();  // 清空跳表
            }
            m_sl.put(timestamp, value);
        }

        /**
         * 写入DataBlock 和 IndexBlock
         * @tparam T
         */
        template <class T>
        void Controller<T>::write_data()
        {
            if (!m_exist_index_meta)  // 没有index_meta
            {

            }
            // 刷入data_block 和 index_entry
            m_tsm.write_data_to_file(pop_data_from_deque(), m_file_path, m_data_offset);
        }

        /**
         * 放置数据
         */
        template <class T>
        void Controller<T>::push_data_to_deque(DataBlock<T> data_block)
        {
            std::lock_guard<std::mutex> lock(m_data_lock);  // 互斥锁
            m_data_deque.push_back(data_block);
        }

        /**
         * 获取数据
         */
        template <class T>
        DataBlock<T> Controller<T>::pop_data_from_deque()
        {
            std::lock_guard<std::mutex> lock(m_data_lock);
            if (!m_data_deque.empty())
            {
                DataBlock<T> data_block = m_data_deque.front();
                m_data_deque.pop_front();
                return data_block;
            }
        }

        template <class T>
        void Controller<T>::push_index_to_deque(IndexEntry index_block)
        {
            std::lock_guard<std::mutex> lock(m_index_lock);
            m_index_deque.push_back(index_block);
        }

        template <class T>
        IndexEntry Controller<T>::pop_index_from_deque()
        {
            std::lock_guard<std::mutex> lock(m_index_lock);
            if (!m_index_deque.empty())
            {
                DataBlock<T> index_block = m_index_deque.front();
                m_index_deque.pop_front();
                return index_block;
            }
        }

        template <class T>
        void Controller<T>::set_file_path(string & file_path)
        {
            m_file_path = file_path;
        }
    }
}