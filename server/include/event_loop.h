
#ifndef DTIMEDB_EVENT_LOOP_H
#define DTIMEDB_EVENT_LOOP_H
#include <hash_set>
#include <hash_map>
#include "event_base.h"
#include "timer_queue.h"
/**
 * 事件类，包括了定时器事件以及io事件的操作
 */
#define MAX_EVENTS 10
class event_loop{
public:
    event_loop();
    //处理事件的函数
    void process_evs();
    void add_ioev(int fd,io_call_back* proc,int mask,void *args= nullptr);
    void del_ioev(int fd,int mask);
    void del_ioev(int fd);
    void n_listening(__gnu_cxx::hash_set<int> &conns){conns = listening;}

    //timer events
    int run_at(time_call_back cb,void *args,uint64_t timestamp);
    int run_every(time_call_back cb,void *args,int sec,int millis=0);
    int run_after(time_call_back cb,void *args,int sec,int mills=0);
    void del_timer(int timer_id);

    //task operate
    void add_task(pendingFunc func,void *args);
    void run_task();
private:
    //epoll文件描述符
    int  _epfd;
    //存储触发的事件
    struct epoll_event _fired_evs[MAX_EVENTS];
    //io事件映射,key是文件描述符，value是io事件
    __gnu_cxx::hash_map<int ,io_event> _io_evs;
    //迭代器，遍历io_evs哈希映射中的键值对
    typedef __gnu_cxx::hash_map<int ,io_event>::iterator io_ev_it;
    timer_queue* _timer_que;
    //待执行任务
    std::vector<std::pair<pendingFunc ,void*>> _pending_func;
    __gnu_cxx::hash_set<int> listening;
    friend void timer_queue_cb(event_loop* loop,int fd,void *args);
};
#endif //DTIMEDB_EVENT_LOOP_H
