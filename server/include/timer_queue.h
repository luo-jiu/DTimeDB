//
// Created by illumwang on 24-2-11.
//

#ifndef DTIMEDB_TIMER_QUEUE_H
#define DTIMEDB_TIMER_QUEUE_H
#include "event_base.h"
#include <hash_map>
class timer_queue{
public:
    timer_queue();
    ~timer_queue();
    int add_timer(timer_event &te);
    void del_timer(int timer_id);
    int notifier()const{return _timer_fd;}
    int size()const{return _time_count;}
    void get_timo(std::vector<timer_event> &fired_event);
private:
    void reset_timo();
    void heap_add(timer_event &te);
    void heap_del(int pos);
    void heap_pop();
    void heap_hold(int pos);

    std::vector<timer_event> _event_lst;
    typedef  std::vector<timer_event>::iterator vit;
    __gnu_cxx::hash_map<int,int> _position;
    typedef  __gnu_cxx::hash_map<int,int>::iterator mit;
    int _time_count;
    // 记录下一个定时器的ID
    int _next_time_id;

    int _timer_fd;

    uint64_t _pioneer;
};
#endif //DTIMEDB_TIMER_QUEUE_H
