#ifndef DTIMEDB_OBSERVER_H
#define DTIMEDB_OBSERVER_H

#include <string>

namespace dt::impl
{
    class ITableStateObserver
    {
    public:
        // 更新状态函数, 具体功能由对应观察者实现
        virtual void update(const std::string & db_name, const std::string & tb_name, const std::string & field_name, bool is_registered, bool use_index_entry_map) = 0;
    };

    class ITableStateSubject
    {
    public:
        // 添加观察者
        virtual void attach(ITableStateObserver * observer) = 0;

        // 移除观察者
        virtual void detach(ITableStateObserver * observer) = 0;

        // 通知所有观察者状态发生变化
        virtual void notify(const std::string & db_name, const std::string & tb_name, const std::string & field_name, bool is_registered, bool use_index_entry_map) = 0;
    };
}

#endif //DTIMEDB_OBSERVER_H
