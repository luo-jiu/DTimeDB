#ifndef DTIMEDB_TABLE_STATUS_H
#define DTIMEDB_TABLE_STATUS_H

#include <engine/impl/iobserver_table_state.h>
using namespace dt::impl;

#include <atomic>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <string>
using std::string;

namespace dt::tsm
{
    class TableState : public ITableStateObserver
    {
    public:
        void update(const string & db_name, const string & tb_name, bool is_registered) override
        {
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
        bool check_and_register(const string & db_name, const string & tb_name)
        {
            std::shared_lock<std::shared_mutex> read_lock(m_mutex);
            auto db_it = m_state_map.find(db_name);
            if (db_it != m_state_map.end()) {
                auto tb_it = db_it->second.find(tb_name);
                if (tb_it != db_it->second.end()) {
                    // 如果已经注册，直接返回
                    return tb_it->second.m_is_registered.load();
                }
            }
            // 升级到写锁
            read_lock.unlock();
            std::unique_lock<std::shared_mutex> write_lock(m_mutex);
            // 再次检查以避免竞态条件
            auto& tb_status = m_state_map[db_name][tb_name];
            if (!tb_status.m_is_registered.load()) {
                tb_status.m_is_registered.store(true);
                return true;
            }
            return false;
        }

        // 移除表的注册状态
        void remove_status(const string & db_name, const string & tb_name)
        {
            std::unique_lock<std::shared_mutex> write_lock(m_mutex);
            auto db_it = m_state_map.find(db_name);
            if (db_it != m_state_map.end()) {
                db_it->second.erase(tb_name);
                if (db_it->second.empty()) {
                    m_state_map.erase(db_name);
                }
            }
        }

        void iterate_map()
        {
            std::vector<std::pair<string, string>> table_to_process;

            // 收集待处理的表
            {
                std::shared_lock<std::shared_mutex> read_lock(m_mutex);
                for (const auto& db_pair : m_state_map)
                {
                    for (const auto& tb_pair : db_pair.second)
                    {
                        if (tb_pair.second.m_is_registered.load())
                        {
                            // 说明有数据, 先存储起来
                            table_to_process.emplace_back(db_pair.first, tb_pair.first);
                        }
                    }
                }
            }  // 释放锁

            // 处理这些表
            for (const auto& item : table_to_process)
            {
                const auto& db_name = item.first;
                const auto& tb_name = item.second;

                // 执行相关操作, 例如让跳表刷新数据到磁盘
                // ...
                std::cout << "监控到跳表注册的事件" << std::endl;

                // 在此执行回调函数
                // ...

                // 移除掉状态
                {
                    std::unique_lock<std::shared_mutex> write_lock(m_mutex);
                    auto db_it = m_state_map.find(db_name);
                    if (db_it != m_state_map.end()) {
                        db_it->second.erase(tb_name);
                        if (db_it->second.empty()) {
                            m_state_map.erase(db_name);
                        }
                    }
                }
            }
        }

    private:
        struct FieldInfo
        {
            std::atomic<bool> m_is_registered;
        };

        struct TableInfo
        {
            std::atomic<bool> m_is_registered;
            std::map<string, FieldInfo> map;
        };

        //       db_name          tb_name
        std::map<string, std::map<string, TableInfo>>      m_state_map;
        mutable std::shared_mutex                          m_mutex;
    };
}

#endif //DTIMEDB_TABLE_STATUS_H
