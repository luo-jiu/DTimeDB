#ifndef DTIMEDB_THREAD_POOL_H
#define DTIMEDB_THREAD_POOL_H

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <future>
#include <condition_variable>
#include <functional>
#include <iostream>

namespace dt::thread
{
    /**
     * 线程池
     *
     * 为存储引擎提供线程
     */
    class ThreadPool
    {
    public:
        ThreadPool(size_t threads): m_stop(false)
        {
            for (size_t i = 0; i < threads; ++i)
            {
                m_workers.emplace_back([this]{
                    while (true)
                    {
                        std::function<void()> task;
                        {
                            std::unique_lock<std::mutex> lock(this->m_queue_mutex);
                            this->m_condition.wait(lock, [this]{ return this->m_stop || !this->m_tasks.empty(); });
                            if (this->m_stop && this->m_tasks.empty())
                            {
                                return;
                            }
                            task = std::move(this->m_tasks.front());
                            this->m_tasks.pop();
                        }
                        task();
                    }
                });
            }
        }

        template<class F, class... Args>
        auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
        {
            using return_type = typename std::result_of<F(Args...)>::type;
            auto task = std::make_shared<std::packaged_task<return_type()>>(
                    std::bind(std::forward<F>(f), std::forward<Args>(args)...)
                    );
            std::future<return_type> res = task->get_future();
            {
                std::unique_lock<std::mutex> lock(m_queue_mutex);
                if (m_stop)
                {
                    throw std::runtime_error("enqueue on stopped ThreadPool");
                }
                m_tasks.emplace([task](){ (*task)(); });
            }
            m_condition.notify_one();
            return res;
        }

        ~ThreadPool()
        {
            {
                std::unique_lock<std::mutex> lock(m_queue_mutex);
                m_stop = true;
            }
            m_condition.notify_all();
            for (std::thread & worker : m_workers)
            {
                worker.join();
            }
        }

    private:
        std::vector<std::thread>                m_workers;
        std::queue<std::function<void()>>       m_tasks;

        std::mutex                              m_queue_mutex;
        std::condition_variable                 m_condition;
        bool                                    m_stop;
    };
}

#endif //DTIMEDB_THREAD_POOL_H
