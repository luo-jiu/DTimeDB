#ifndef DTIMEDB_OBSERVER_H
#define DTIMEDB_OBSERVER_H

#include <string>
using std::string;

namespace dt::impl
{
    class ITableStateObserver
    {
    public:
        // 更新状态函数, 具体功能由对应观察者实现
        virtual void update(const string& db_name, const string& tb_name, bool is_registered) = 0;
    };

    class ITableStateSubject
    {
    public:
        // 添加观察者
        virtual void attach(ITableStateObserver* observer) = 0;

        // 移除观察者
        virtual void detach(ITableStateObserver* observer) = 0;

        // 通知所有观察者状态发生变化
        virtual void notify(const string& db_name, const string& tb_name, bool is_registered) = 0;
    };
}

#endif //DTIMEDB_OBSERVER_H
