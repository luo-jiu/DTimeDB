#pragma once

#include <engine/impl/iobserver_table_state.h>
using namespace dt::impl;

#include <vector>
#include <cstdlib>
#include <iostream>
#include <shared_mutex>
#include <chrono>
using namespace std::chrono;

namespace dt::tsm
{
    template <class T>
    class SkipListIterator;

    const int MAX_LEVEL = 32;
    const float PROBABILITY = 0.5;

    /**
     * 跳表定义
     * 线程不安全
     */
    template <class T>
    class SkipList : public ITableStateSubject
    {
    public:
        SkipList()
        {
            m_head = new Node(high_resolution_clock::time_point(), T(), MAX_LEVEL);
            srand(static_cast<unsigned int>(time(nullptr)));  // 为伪随机数生成器设置一个种子值
            m_bottom_level_node_count = 0;
        }

        ~SkipList()
        {
            Node* current = m_head;
            while (current != nullptr)
            {
                Node* next = current->m_nexts[0];
                delete current;
                current = next;
            }
        }

        void attach(ITableStateObserver * observer) override
        {
            std::unique_lock<std::shared_mutex> lock(m_mutex);
            m_observers.push_back(observer);
        }

        void detach(ITableStateObserver * observer) override
        {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            m_observers.remove(observer);
        }

        void notify(const string & db_name, const string & tb_name, bool is_registered) override
        {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            // 通知所有观察者发生变化
            for (auto observer : m_observers)
            {
                observer->update(db_name, tb_name, is_registered);
            }
        }

        SkipListIterator<T> begin() const
        {
            return SkipListIterator<T>(m_head->m_nexts[0]);  // 从最低层开始迭代
        }

        SkipListIterator<T> end() const
        {
            return SkipListIterator<T>(nullptr);  // 迭代结束标志
        }

        void put(high_resolution_clock::time_point key, T & value);
        bool get(high_resolution_clock::time_point key, T & value);
        void del(high_resolution_clock::time_point key);
        void cle();

        bool empty();

        high_resolution_clock::time_point min_key();  // 获取最小键
        high_resolution_clock::time_point max_key();  // 获取最大键

        void traverse();

        size_t size();


    public:
        struct Node
        {
            high_resolution_clock::time_point m_key;
            T m_value;
            std::vector<Node*> m_nexts;

            Node(high_resolution_clock::time_point key, T value, int level): m_key(key), m_value(value), m_nexts(level, nullptr) {}
        };

    private:
        Node*                                   m_head;  // 头结点
        size_t                                  m_bottom_level_node_count; // 计数器
        std::list<ITableStateObserver*>         m_observers;
        mutable std::shared_mutex               m_mutex;  // 读写锁
        std::mutex                              m_sl_mutex;  // 互斥锁, 性能瓶颈, 未来再说

        int random_level()
        {
            int level = 1;
            while (rand() % 100 < (PROBABILITY * 100) && level < MAX_LEVEL)
            {
                level++;
            }
            return level;
        }
    };

    /**
     * 跳表迭代器
     * @tparam T
     */
    template <class T>
    class SkipListIterator
    {
    public:
        SkipListIterator(typename SkipList<T>::Node* start): current(start) {}

        bool operator!=(const SkipListIterator& other) const
        {
            return current != other.current;
        }

        SkipListIterator& operator++()
        {
            if (current != nullptr)
            {
                current = current->m_nexts[0];
            }
            return *this;
        }

        const std::pair<high_resolution_clock::time_point, T>& operator*() const
        {
            if (current == nullptr)
            {
                throw std::runtime_error("Attempted to dereference a null iterator");
            }
            static std::pair<high_resolution_clock::time_point, T> placeholder;
            placeholder = {current->m_key, current->m_value};
            return placeholder;
        }

    private:
        typename SkipList<T>::Node* current;
    };

    template <class T>
    void SkipList<T>::put(high_resolution_clock::time_point key, T& value)
    {
        std::lock_guard<std::mutex> lock(m_sl_mutex);  // 互斥锁
        std::vector<Node*> update(MAX_LEVEL, nullptr);
        Node* current = m_head;

        for (int i = MAX_LEVEL - 1; i >= 0; --i)
        {
            while (current->m_nexts[i] != nullptr && current->m_nexts[i]->m_key < key)
            {
                current = current->m_nexts[i];
            }
            update[i] = current;  // 更新当前层最后一个小于新键的节点
        }

        current = current->m_nexts[0];
        if (current != nullptr && current->m_key == key)
        {
            value = current->m_value;  // 如果键已存在，更新值
            return;
        }

        int level = random_level();
        if (level > MAX_LEVEL)
        {
            level = MAX_LEVEL;
        }

        Node* new_node = new Node(key, value, level);
        for (int i = 0; i < level; ++i)  // 只在新节点存在的层级中插入
        {
            new_node->m_nexts[i] = update[i]->m_nexts[i];
            update[i]->m_nexts[i] = new_node;
        }

        if (level >= 1) ++m_bottom_level_node_count;  // 计数器 + 1
    }

    template <class T>
    bool SkipList<T>::get(high_resolution_clock::time_point key, T& value)
    {
        std::lock_guard<std::mutex> lock(m_sl_mutex);  // 互斥锁
        Node* current = m_head;
        for (int i = MAX_LEVEL - 1; i >= 0; --i)
        {
            while (current->m_nexts[i] != nullptr && current->m_nexts[i]->m_key < key)
            {
                current = current->m_nexts[i];
            }
        }

        current = current->m_nexts[0];
        if (current != nullptr && current->m_key == key)
        {
            value = current->m_value;
            return true;
        }
        return false;
    }

    template <class T>
    void SkipList<T>::del(high_resolution_clock::time_point key)
    {
        std::lock_guard<std::mutex> lock(m_sl_mutex);  // 互斥锁
        std::vector<Node*> update(MAX_LEVEL, nullptr);
        Node* current = m_head;

        for (int i = MAX_LEVEL - 1; i >= 0; --i)
        {
            while (current->m_nexts[i] != nullptr && current->m_nexts[i]->m_key < key)
            {
                current = current->m_nexts[i];
            }
            update[i] = current;
        }

        current = current->m_nexts[0];

        if (current != nullptr && current->m_key == key)
        {
            for (int i = 0; i < MAX_LEVEL; ++i)
            {
                if (update[i]->m_nexts[i] != current)
                {
                    break;
                }
                update[i]->m_nexts[i] = current->m_nexts[i];
            }
            delete current;
            --m_bottom_level_node_count;
        }
    }

    /**
     * 清空调跳表
     */
    template <class T>
    void SkipList<T>::cle()
    {
        std::lock_guard<std::mutex> lock(m_sl_mutex);  // 互斥锁
        for (int i = 0; i < MAX_LEVEL; ++i) {
            m_head->m_nexts[i] = nullptr;
        }

        Node* current = m_head->m_nexts[0];
        while (current != nullptr)
        {
            Node* next = current->m_nexts[0];
            delete current;
            current = next;
        }

        // 重新初始化跳表状态
        m_head->m_nexts.assign(MAX_LEVEL, nullptr);
        m_bottom_level_node_count = 0;
    }

    template <class T>
    bool SkipList<T>::empty()
    {
        return !m_bottom_level_node_count;
    }

    template <class T>
    void SkipList<T>::traverse()
    {
        Node* current = m_head->m_nexts[0];
        std::cout << "SkipList Data:" << std::endl;
        while (current != nullptr)
        {
            // 处理当前接节点
            std::cout << "(" << current->m_key.time_since_epoch().count() << ", " << current->m_value << ") " << '\n';
            current = current->m_nexts[0];
        }
        std::cout << std::endl;
    }

    /**
     * 获取跳表大小
     */
    template <class T>
    size_t SkipList<T>::size()
    {
        std::lock_guard<std::mutex> lock(m_sl_mutex);  // 互斥锁
        return m_bottom_level_node_count;
    }

    /**
     * 获取最小的键
     */
    template <class T>
    high_resolution_clock::time_point SkipList<T>::min_key()
    {
        std::lock_guard<std::mutex> lock(m_sl_mutex);  // 互斥锁
        if (m_head->m_nexts[0] != nullptr)
        {
            return m_head->m_nexts[0]->m_key;
        }
        return {};
    }

    /**
     * 获取最大的键
     */
    template <class T>
    high_resolution_clock::time_point SkipList<T>::max_key()
    {
        std::lock_guard<std::mutex> lock(m_sl_mutex);  // 互斥锁
        Node* current = m_head;
        while (current->m_nexts[0] != nullptr)
        {
            current = current->m_nexts[0];
        }
        if (current != m_head)
        {
            return current->m_key;
        }
        return {};
    }
}