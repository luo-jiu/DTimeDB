#ifndef DTIMEDB_TABLE_STATUS_H
#define DTIMEDB_TABLE_STATUS_H

#include <atomic>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <string>
using std::string;

namespace dt::tsm
{
    class TableState
    {
    public:
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

        void iterate_map() const
        {
            std::shared_lock<std::shared_mutex> read_lock(m_mutex);
            for (const auto& db_pair : m_state_map)
            {
                for (const auto& tb_pair : db_pair.second)
                {
                    // 说明有数据


                }
            }
//            std::cout << "监控线程监控数据中..." << "\n";
        }

        const auto& get_map()
        {
            return m_state_map;
        }

    private:
        struct TableInfo
        {
            std::atomic<bool> m_is_registered;
        };

        //       db_name          tb_name
        std::map<string, std::map<string, TableInfo>>      m_state_map;
        mutable std::shared_mutex                          m_mutex;
    };
}

#endif //DTIMEDB_TABLE_STATUS_H
