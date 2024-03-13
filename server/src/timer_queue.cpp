//
// Created by illumwang on 24-2-12.
//
#include "server/include/timer_queue.h"
#include "server/include/event_base.h"
#include <sys/timerfd.h>
#include <iostream>
#include <unistd.h>
#include <strings.h>
#include <pthread.h>
timer_queue::timer_queue():_time_count(0),_next_time_id(0),_pioneer(-1) {
    _timer_fd=::timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK | TFD_CLOEXEC);
    if (_timer_fd==-1){
        std::cout<<"ERROR: FD_CREATE"<<std::endl;
    }
}
timer_queue::~timer_queue() {
    ::close(_timer_fd);
}

int timer_queue::add_timer(timer_event &te) {
    te.timer_id=_next_time_id++;
    heap_add(te);
    if (_event_lst[0].timestamp<_pioneer){
        _pioneer=_event_lst[0].timestamp;
        reset_timo();
    }
    return te.timer_id;
}

void timer_queue::del_timer(int timer_id) {
    mit it=_position.find(timer_id);
    if (it==_position.end()){
        std::cout<<"ERROR: FD_DEL"<<std::endl;
        return;
    }
    int pos=it->second;
    heap_del(pos);
    if (_time_count==0){
        _pioneer=-1;
        reset_timo();
    } else if (_event_lst[0].timestamp < _pioneer)
    {
        _pioneer = _event_lst[0].timestamp;
        reset_timo();
    }
}

void timer_queue::get_timo(std::vector<timer_event> &fired_event) {
    std::vector<timer_event> _reuse_lst;
    while (_time_count!=0&&_pioneer==_event_lst[0].timestamp){
        timer_event te=_event_lst[0];
        fired_event.push_back(te);
        if (te.interval){
            te.timestamp+=te.interval;
            _reuse_lst.push_back(te);
        }
        heap_pop();
    }
}

void timer_queue::reset_timo() {
    struct itimerspec old_timestamp,new_timestamp;
    ::bzero(&new_timestamp, sizeof(new_timestamp));
    if (_pioneer!=(uint64_t)-1){
        new_timestamp.it_value.tv_sec=_pioneer/1000;
        new_timestamp.it_value.tv_nsec=(_pioneer%1000)*1000000;
    }
    ::timerfd_settime(_timer_fd,TFD_TIMER_ABSTIME,&new_timestamp,&old_timestamp);
}

void timer_queue::heap_add(timer_event &te) {
    _event_lst.push_back(te);
    _position[te.timer_id]=_time_count;
    int curr_pos=_time_count;
    _time_count++;
    int prt_pos=(curr_pos - 1 ) / 2;
    while (prt_pos>=0&&_event_lst[curr_pos].timestamp < _event_lst[prt_pos].timestamp){
        timer_event tmp=_event_lst[curr_pos];
        _event_lst[curr_pos]=_event_lst[prt_pos];
        _event_lst[prt_pos]=tmp;
        // update position
        _position[_event_lst[curr_pos].timer_id]=curr_pos;
        _position[tmp.timer_id]=prt_pos;
        curr_pos=prt_pos;
        prt_pos=(curr_pos - 1) / 2;
    }
}

void timer_queue::heap_del(int pos) {
    timer_event to_del=_event_lst[pos];
    timer_event tmp=_event_lst[_time_count-1];
    _event_lst[pos]=tmp;
    _position[tmp.timer_id]=pos;
    _position.erase(to_del.timer_id);
    _time_count--;
    _event_lst.pop_back();
    heap_hold(pos);
}

void timer_queue::heap_hold(int pos) {
    int left = 2 * pos + 1, right = 2 * pos + 2;
    int min_pos = pos;
    if (left < _time_count && _event_lst[min_pos].timestamp > _event_lst[left].timestamp){
        min_pos = left;
    }
    if (right < _time_count && _event_lst[min_pos].timestamp > _event_lst[right].timestamp){
        min_pos = right;
    }
    if (min_pos != pos){
        timer_event tmp = _event_lst[min_pos];
        _event_lst[min_pos] = _event_lst[pos];
        _event_lst[pos] = tmp;
        _position[_event_lst[min_pos].timer_id] = min_pos;
        _position[tmp.timer_id] = pos;
        heap_hold(pos);
    }
}

void timer_queue::heap_pop() {
    if (_time_count <=0 ){
        return;
    }
    _position.erase(_event_lst[0].timer_id);
    if (_time_count > 1){
        timer_event tmp=_event_lst[_time_count-1];
        _event_lst[0] = tmp;
        _position[tmp.timer_id] = 0;
        _event_lst.pop_back();
        _time_count--;
        heap_hold(0);
    }else if (_time_count == 1){
        _event_lst.clear();
        _time_count = 0;
    }
}