//
// Created by illumwang on 24-2-25.
//

#ifndef DTIMEDB_THREAD_QUEUE_H
#define DTIMEDB_THREAD_QUEUE_H
#include "server_header.h"
#include <queue>
template <typename T>
class thread_queue{
public:
    void send_msg(){

    }
    void recv_msg(){

    }
private:
    int _evfd;
    event_loop*  _loop;
    std::queue<T> queue;
    pthread_mutex_t  _mutex;
};
#endif //DTIMEDB_THREAD_QUEUE_H
