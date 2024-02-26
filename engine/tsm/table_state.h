#ifndef DTIMEDB_TABLE_STATE_H
#define DTIMEDB_TABLE_STATE_H

#include "engine/impl/iobserver_table_state.h"

#include <functional>
#include <atomic>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <utility>

namespace dt::tsm
{
    /**
     * 表级别的状态更新
     * data block刷盘
     */
    class TableState : public impl::ITableStateObserver
    {
    public:
        using DataBlockCallback = std::function<bool(const std::string&, const std::string&)>;

        // 设置回调函数
        void set_condition_callback(DataBlockCallback callback)
        {
            m_condition_callback = std::move(callback);
        }

        void update(
                const std::string & db_name,
                const std::string & tb_name,
                const std::string & shard_id,
                const std::string & field_name,
                bool is_registered,
                bool use_index_entry_map) override
        {
            // 防止误注册index entry 事件[因为这两个共享一个field/notify 函数]
            // 但是注册index entry,use_index_entry_map 肯定是true, 固然用这个判断是不是误注册
            if (use_index_entry_map) return;
            // 更新状态逻辑
            if (is_registered)
            {
                check_and_register(db_name, tb_name);
            }
            else
            {
                remove_status(db_name, tb_name);
            }
        }

        // 检查并注册表
        bool check_and_register(const std::string & db_name, const std::string & tb_name)
        {
            std::shared_lock<std::shared_mutex> read_lock(m_mutex);
            // 检查数据库和表是否存在
            auto db_it = m_state_map.find(db_name);
            if (db_it != m_state_map.end())
            {
                auto tb_it = db_it->second.find(tb_name);
                if (tb_it != db_it->second.end())
                {
                    // 检查字段是否已经注册,如果已经注册,直接返回
                    return tb_it->second.load();
                }
            }
            // 升级到写锁
            read_lock.unlock();
            std::unique_lock<std::shared_mutex> write_lock(m_mutex);
            // 再次检查以避免在释放读锁和获取写锁的间隙中发生的竞态条件
            auto& tb_map = m_state_map[db_name];
            // 如果字段未注册，则进行注册
            if (!tb_map[tb_name].load())
            {
                tb_map[tb_name].store(true);
                // 此时已经有任务进来了,为其开启消费者线程,调用回调函数
                return true;
            }
            // 字段已注册
            return false;
        }

        // 移除表的注册状态
        void remove_status(const std::string & db_name, const std::string & tb_name)
        {
            std::unique_lock<std::shared_mutex> write_lock(m_mutex);
            auto db_it = m_state_map.find(db_name);
            if (db_it != m_state_map.end())
            {
                db_it->second.erase(tb_name);
                if (db_it->second.empty())
                {
                    m_state_map.erase(db_name);
                }
            }
        }

        void iterate_map() {
            std::vector<std::pair<std::string, std::string>> items_to_process;
            // 收集待处理的数据库、表和字段
            {
                std::shared_lock<std::shared_mutex> read_lock(m_mutex);
                for (const auto& db_pair : m_state_map)
                {
                    const auto& db_name = db_pair.first;
                    for (const auto& tb_pair : db_pair.second)
                    {
                        const auto& tb_name = tb_pair.first;
                        if (tb_pair.second.load()) // 检查表是否已注册
                        {
                            // 该表已注册，先存储起来
                            items_to_process.emplace_back(db_name, tb_name);
                        }
                    }
                }
            }  // 释放读锁

            // 处理这些数据库、表和字段
            for (const auto& item : items_to_process)
            {
                const auto& db_name = std::get<0>(item);
                const auto& tb_name = std::get<1>(item);

                // 执行相关操作，例如更新字段的状态或刷新数据到磁盘
                // ...
                std::cout << "监控线程观测到队列:" << db_name << "，表：" << tb_name << std::endl;

                // 执行回调函数获取时间
                if (m_condition_callback(db_name, tb_name))
                {
                    // 移除满足条件的字段状态
                    remove_status(db_name, tb_name);
                }
            }
        }

    private:
        DataBlockCallback m_condition_callback;  // 存储回调函数


        //       db_name          tb_name
        std::map<std::string, std::map<std::string, std::atomic<bool>>>      m_state_map;
        mutable std::shared_mutex                                            m_mutex;
    };
}

#endif //DTIMEDB_TABLE_STATE_H
