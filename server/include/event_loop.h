
#ifndef DTIMEDB_EVENT_LOOP_H
#define DTIMEDB_EVENT_LOOP_H
#include <hash_set>
#include <hash_map>
#include "event_base.h"
#include "timer_queue.h"

#define MAX_EVENTS 10
class event_loop{
public:
    event_loop();
    void process_evs();
    void add_ioev(int fd,io_call_back* proc,int mask,void *args= nullptr);
    void del_ioev(int fd,int mask);
private:
    int  _epfd;
    struct epoll_event _fired_evs[MAX_EVENTS];
    __gnu_cxx::hash_map<int ,io_event> _io_evs;
    typedef __gnu_cxx::hash_map<int ,io_event>::iterator io_ev_it;
    timer_queue* _timer_que;
    std::vector<std::pair<pendingFunc ,void*>> _pending_func;
    __gnu_cxx::hash_set<int> listening;
    friend void timer_queue_cb(event_loop* loop,int fd,void *args);
};
#endif //DTIMEDB_EVENT_LOOP_H
