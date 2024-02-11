
#ifndef DTIMEDB_EVENT_LOOP_H
#define DTIMEDB_EVENT_LOOP_H
#include <hash_set>
#include <hash_map>
#include "event_base.h"

#define MAX_EVENTS 10
class event_loop{
public:
    event_loop();

private:
    int  _epfd;
    struct epoll_event _fired_evs[MAX_EVENTS];
    __gnu_cxx::hash_map<int ,io_event> _io_evs;
    typedef __gnu_cxx::hash_map<int ,io_event>::iterator io_ev_it;

};
#endif //DTIMEDB_EVENT_LOOP_H
