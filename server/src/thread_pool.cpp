//
// Created by illumwang on 24-2-18.
//
#include "server/include/thread_pool.h"
//消息到达，当消息队列中有数据来的时候，调用这个函数处理消息类型
void msg_comming_cb(event_loop* loop,int fd,void *args){

}

thread_pool::thread_pool(int thread_cnt)
{

}

void thread_pool::run_task(pendingFunc task, void *args)
{

}

void thread_pool::run_task(int thd_index, pendingFunc task, void *args)
{

}

thread_queue <queue_msg> *thread_pool::get_next_thread()
{

}