//
// Created by illumwang on 24-2-10.
//

#ifndef DTIMEDB_EVENT_BASE_H
#define DTIMEDB_EVENT_BASE_H
#include <stdint.h>
class event_loop;
//IO回调函数
typedef void io_call_back(event_loop *loop,int fd,void  *args);
typedef void  time_call_back(event_loop *loop,void *usr_data);
typedef void (*pendingFunc) (event_loop*,void *);
//注册新的event事件
struct io_event{
    io_event():read_cb(nullptr),write_cb(nullptr),read_args(nullptr),write_args(nullptr){}
    int mask;
    io_call_back *read_cb;
    io_call_back *write_cb;
    void *read_args;
    void *write_args;

};
//定时器，超时处理或任务调度等
struct timer_event{
    timer_event(time_call_back *_time_cb,void *data,uint64_t arg_ts,uint32_t arg_int = 0)
    : time_cb(_time_cb), cb_data(data), timestamp(arg_ts), interval(arg_int)
    {}
    time_call_back *time_cb;
    void *cb_data;
    uint64_t timestamp;
    uint32_t interval;
    int timer_id{};
};
#endif //DTIMEDB_EVENT_BASE_H
