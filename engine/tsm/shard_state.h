#ifndef DTIMEDB_SHARD_STATE_H
#define DTIMEDB_SHARD_STATE_H

#include "engine/impl/iobserver_shard_state.h"

#include <unordered_map>
#include <atomic>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <chrono>

namespace dt::tsm
{
    class ShardState : public impl::IShardStateObserver
    {
    public:
        using ConditionCallback = std::function<bool()>;

        void set_condition_callback(ConditionCallback callback)
        {
            m_condition_callback = std::move(callback);
        }

        void shard_update(bool is_registered) override
        {
            // 状态更新
            if (is_registered)
            {
                check_and_register();
            }
            else
            {
                remove_status();
            }
        }

        bool check_and_register()
        {
            std::unique_lock<std::shared_mutex> write_lock(m_mutex);
            if (!m_is_registered.load())  // 如果没注册
            {
                m_is_registered.store(true);
            }
            m_change_total_count++;  // 计数器+1
            return true;
        }

        void remove_status()
        {
            std::unique_lock<std::shared_mutex> write_lock(m_mutex);
            m_is_registered.store(false);
            m_change_total_count = 0;
        }

        void iterate_map()
        {
            auto curr_time = std::chrono::system_clock::now();
            std::unique_lock<std::shared_mutex> write_lock(m_mutex);
            if (m_is_registered.load())  // 有事件注册
            {
                std::cout << "监控到shard刷盘事件注册\n";
                // 时间超过5 秒或者shard 变化次数达到10 次
                if (curr_time - m_last_time >= std::chrono::seconds(5) || m_change_total_count >= 10)
                {
                    if (m_condition_callback())
                    {
                        m_is_registered.store(false);
                        m_change_total_count = 0;
                    }
                }
            }
        }

    private:
        mutable std::shared_mutex                           m_mutex;
        ConditionCallback                                   m_condition_callback;

        std::chrono::high_resolution_clock::time_point      m_last_time;
        std::atomic<bool>                                   m_is_registered{false};
        size_t                                              m_change_total_count;
    };
}

#endif //DTIMEDB_SHARD_STATE_H
