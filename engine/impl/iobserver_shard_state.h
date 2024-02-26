#ifndef DTIMEDB_IOBSERVER_SHARD_STATE_H
#define DTIMEDB_IOBSERVER_SHARD_STATE_H

#include <string>
#include <chrono>

namespace dt::impl
{
    class IShardStateObserver
    {
    public:
        // 更新状态函数, 具体功能由对应观察者实现
        virtual void shard_update(bool is_registered) = 0;
    };

    class IShardStateSubject
    {
    public:
        // 添加观察者
        virtual void shard_attach(IShardStateObserver * observer) = 0;

        // 移除观察者
        virtual void shard_detach(IShardStateObserver * observer) = 0;

        // 通知所有观察者状态发生变化
        virtual void shard_notify(bool is_registered) = 0;
    };
}

#endif //DTIMEDB_IOBSERVER_SHARD_STATE_H
