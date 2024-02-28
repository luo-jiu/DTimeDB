#ifndef DTIMEDB_TABLE_STATUS_H
#define DTIMEDB_TABLE_STATUS_H

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
    struct Info
    {
        std::string m_db_name;
        std::string m_tb_name;
        std::string m_shard_id;
        std::string m_field_name;
    };

    /**
     * 字段级别的状态更新
     * skip_list, index entry
     */
    class FieldState : public impl::ITableStateObserver
    {
    public:
        using ConditionCallback = std::function<bool(const std::string&, const std::string&, const std::string&, const std::string&)>;

        // 设置回调函数
        void set_skip_condition_callback(ConditionCallback callback)
        {
            m_skip_condition_callback = std::move(callback);
        }
        void set_index_condition_callback(ConditionCallback callback)
        {
            m_index_condition_callback = std::move(callback);
        }

        void update(
                const std::string & db_name,
                const std::string & tb_name,
                const std::string & shard_id,
                const std::string & field_name,
                bool is_registered,
                bool use_index_entry_map) override
        {
            // 更新状态逻辑
            if (is_registered)
            {
                check_and_register(db_name, tb_name, shard_id, field_name, use_index_entry_map);
            }
            else
            {
                remove_status(db_name, tb_name, field_name, use_index_entry_map);
            }
        }

        // 检查并注册表
        bool check_and_register(
                const std::string & db_name,
                const std::string & tb_name,
                const std::string & shard_id,
                const std::string & field_name,
                bool use_index_entry_map)
        {
            auto & state_map = use_index_entry_map ? m_index_entry_state_map : m_skip_list_state_map;
            std::shared_mutex & map_mutex = use_index_entry_map ? m_index_entry_mutex : m_skip_list_mutex;
            std::shared_lock<std::shared_mutex> read_lock(map_mutex);
            // 检查数据库和表是否存在
            auto db_it = state_map.find(db_name);
            if (db_it != state_map.end())
            {
                auto tb_it = db_it->second.find(tb_name);
                if (tb_it != db_it->second.end())
                {
                    // 检查字段是否已经注册
                    auto field_it = tb_it->second.m_field_map.find(field_name);
                    if (field_it != tb_it->second.m_field_map.end())
                    {
                        // 如果已经注册，直接返回
                        return field_it->second.m_is_registered.load();
                    }
                }
            }
            // 升级到写锁
            read_lock.unlock();
            std::unique_lock<std::shared_mutex> write_lock(map_mutex);
            // 再次检查以避免在释放读锁和获取写锁的间隙中发生的竞态条件
            auto& tb_map = state_map[db_name];
            auto& field_status = tb_map[tb_name].m_field_map[field_name];
            // 如果字段未注册，则进行注册
            if (!field_status.m_is_registered.load())
            {
                field_status.m_is_registered.store(true);
                field_status.m_shard_id = shard_id;
                return true;
            }
            // 字段已注册
            return false;
        }

        // 移除表的注册状态
        void remove_status(
                const std::string & db_name,
                const std::string & tb_name,
                const std::string & field_name,
                bool use_index_entry_map)
        {
            auto & state_map = use_index_entry_map ? m_index_entry_state_map : m_skip_list_state_map;
            std::shared_mutex & map_mutex = use_index_entry_map ? m_index_entry_mutex : m_skip_list_mutex;
            std::unique_lock<std::shared_mutex> write_lock(map_mutex);
            auto db_it = state_map.find(db_name);
            if (db_it != state_map.end())
            {
                auto& tb_info = db_it->second[tb_name];
                tb_info.m_field_map.erase(field_name);
                if (tb_info.m_field_map.empty())
                {
                    db_it->second.erase(tb_name);
                    if (db_it->second.empty())
                    {
                        state_map.erase(db_name);
                    }
                }
            }
        }

        void iterate_map(bool use_index_entry_map)
        {
            auto & state_map = use_index_entry_map ? m_index_entry_state_map : m_skip_list_state_map;
            std::shared_mutex & map_mutex = use_index_entry_map ? m_index_entry_mutex : m_skip_list_mutex;
            std::vector<Info> items_to_process;
            // 收集待处理的数据库、表和字段
            {
                std::shared_lock<std::shared_mutex> read_lock(map_mutex);
                for (const auto& db_pair : state_map)
                {
                    const auto& db_name = db_pair.first;
                    for (const auto& tb_pair : db_pair.second)
                    {
                        const auto& tb_name = tb_pair.first;
                        for (const auto& field_pair : tb_pair.second.m_field_map)
                        {
                            const auto& field_name = field_pair.first;
                            if (field_pair.second.m_is_registered.load())
                            {
                                // 说明该字段已注册，先存储起来
                                items_to_process.emplace_back(Info{db_name, tb_name, field_pair.second.m_shard_id, field_name});
                            }
                        }
                    }
                }
            }  // 释放读锁

            // 处理这些数据库、表和字段
            for (const auto& item : items_to_process)
            {
                const auto& db_name = item.m_db_name;
                const auto& tb_name = item.m_tb_name;
                const auto& shard_id = item.m_shard_id;
                const auto& field_name = item.m_field_name;

                // 执行相关操作，例如更新字段的状态或刷新数据到磁盘
                // ...
                if (!use_index_entry_map)
                {
//                    std::cout << "监控到跳表注册事件(skip_list)：" << db_name << "，表：" << tb_name << "，字段：" << field_name << std::endl;
                    // 执行回调函数获取时间
                    if (m_skip_condition_callback && m_skip_condition_callback(db_name, tb_name, shard_id, field_name))
                    {
                        // 添加到移除列表
                        remove_status(db_name, tb_name, field_name, use_index_entry_map);
                        std::cout << "监控到跳表数据符合刷盘条件：" << db_name << "，表：" << tb_name << "，字段：" << field_name << std::endl;
                    }
                }
                else
                {
//                    std::cout << "监控到跳表注册事件(index_entry)：" << db_name << "，表：" << tb_name << "，字段：" << field_name << std::endl;
                    // 执行回调函数获取时间
                    if (m_index_condition_callback && m_index_condition_callback(db_name, tb_name, shard_id, field_name))
                    {
                        // 添加到移除列表
                        remove_status(db_name, tb_name, field_name, use_index_entry_map);
                        std::cout << "监控到跳表数据符合刷盘条件：" << db_name << "，表：" << tb_name << "，字段：" << field_name << std::endl;
                    }
                }
            }
        }

    private:
        ConditionCallback m_skip_condition_callback;  // 存储回调函数
        ConditionCallback m_index_condition_callback;

        struct FieldInfo
        {
            std::string m_shard_id;
            std::atomic<bool> m_is_registered{};
        };

        struct TableInfo
        {
            std::map<std::string, FieldInfo> m_field_map;  // 映射到对应的跳表
        };

        //       db_name          tb_name
        std::map<std::string, std::map<std::string, TableInfo>>      m_skip_list_state_map;
        std::map<std::string, std::map<std::string, TableInfo>>      m_index_entry_state_map;
        mutable std::shared_mutex                                    m_skip_list_mutex;
        mutable std::shared_mutex                                    m_index_entry_mutex;
    };
}

#endif //DTIMEDB_TABLE_STATUS_H
