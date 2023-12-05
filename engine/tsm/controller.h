#pragma once

#include <memory>
#include <thread>

#include <engine/tsm/skip_list.h>
#include <engine/tsm/tsm.h>

namespace dt
{
    namespace tsm
    {
        /**
         * 控制器，控制写入TSM
         * 实质就是调用各种函数
         */
        template <class T>
        class Controller
        {
        public:
            Controller(): m_sl(), m_tsm(){
            }
            ~Controller() = default;

            void write(high_resolution_clock::time_point timestamp, T value);

        private:
            TSM                                             m_tsm;
            SkipList<T>                                     m_sl;

            std::unique_ptr<DataBlock<T>>                   m_current_data;
//            std::vector<std::unique_ptr<DataBlock<T>>>      m_blocks; // 待刷入块
//            std::vector<std::unique_ptr<IndexEntry>>        m_entry;
        };

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


                // 开启新的线程写入数据
                std::thread write_thread([current_data = std::move(m_current_data),
                                          index_entry = std::unique_ptr<IndexEntry>(m_tsm.create_index_entry(m_sl.max_key(), m_sl.min_key(), 0, m_current_data->m_size)), this]{
                    // 刷入磁盘
                    m_tsm.write_data_to_file(*current_data);

                });
                write_thread.detach();  // 确保线程可以独立运行

                m_current_data = std::make_unique<DataBlock<T>>();  // 重置m_current_data 以便接收新数据
                m_sl.cle();  // 清空跳表
            }
            m_sl.put(timestamp, value);
        }
    }
}