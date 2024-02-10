
#ifndef DTIMEDB_EVENT_LOOP_H
#define DTIMEDB_EVENT_LOOP_H
#include <hash_set>
#include <hash_map>
#define MAX_EVENTS 10
class event_loop{
public:
    event_loop();

private:
    int  _epfd;
    struct epoll_event _fired_evs[MAX_EVENTS];
    hash_map<int ,io_event>
};
#endif //DTIMEDB_EVENT_LOOP_H
