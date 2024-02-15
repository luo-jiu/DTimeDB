
#include "server/include/event_loop.h"
#include "server/include/print_error.h"
#include <sys/epoll.h>
void timer_queue_cb(event_loop* loop,int fd,void *args){
    std::vector<timer_event> fired_evs;
    loop->_timer_que->get_timo(fired_evs);
    for (std::vector<timer_event>::iterator it = fired_evs.begin();
            it != fired_evs.end(); ++it) {
        it->time_cb(loop,it->cb_data);
    }
}
event_loop::event_loop() {
    _epfd = ::epoll_create1(0);
    exit_if(_epfd==-1,"error happened when epoll_create1()");
    _timer_que=new timer_queue();
    exit_if(_timer_que== nullptr,"error happened when timer create");
    add_ioev(_timer_que->notifier(),timer_queue_cb,EPOLLIN,_timer_que);
}
