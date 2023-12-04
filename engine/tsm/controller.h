#pragma once

#include <memory>

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
            Controller(): m_block_count(0) {
                SkipList<T> list;
                m_sl = list;
            }

            ~Controller() = default;

            void write(high_resolution_clock::time_point timestamp, T value);
            bool save_data();

            void cle()
            {
                m_sl.cle();
            }

        private:
            TSM                                             m_tsm;
            SkipList<T>                                     m_sl;

            u_int32_t                                       m_block_count;  // 计数器
            u_int32_t                                       m_index_entry_count;
            std::unique_ptr<DataBlock<T>>                   m_current_data;
            std::vector<std::unique_ptr<DataBlock<T>>>      m_blocks; // 待刷入块
            std::vector<std::unique_ptr<IndexEntry>>        m_entry;
        };

        template <class T>
        void Controller<T>::write(high_resolution_clock::time_point timestamp, T value)
        {
            if (m_sl.size() > 10)  // 跳表数量大于 10
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

                auto indexEntryRawPtr = new IndexEntry(m_tsm.create_index_entry(m_sl.max_key(), m_sl.max_key(), 0, m_current_data->m_size));
                std::unique_ptr<IndexEntry> index_entry(indexEntryRawPtr);

                m_entry.push_back(std::move(index_entry));  // 将IndexEntry 存放到list
                m_blocks.push_back(std::move(m_current_data));  // 将DataBlock 存放到list

                m_sl.cle();  // 清空跳表
            }
            m_sl.put(timestamp, value);
        }

        /**
         * IndexBlockMeta
         */
//        template <class T>
//        bool Controller<T>::save_data()
//        {
//            if (m_current_data > 10)
//            {
//
//            }
//
//        }
    }
}