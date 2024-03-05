/**
 * 线程池类
 * 线程池中有多个线程队列
 */
#ifndef DTIMEDB_THREAD_POOL_H
#define DTIMEDB_THREAD_POOL_H
#include "../include/server_header.h"
//线程的入口函数
extern void* thread_domain(void* args);
class thread_pool{
public:
    thread_pool(int thread_cnt);

    // 获取下一个可用的线程队列
    thread_queue<queue_msg>* get_next_thread();

    // 在指定的线程索引处运行指定的任务
    void run_task(int thd_index, pendingFunc task, void* args = NULL);

    // 在下一个可用的线程中运行指定的任务
    void run_task(pendingFunc task, void* args = NULL);

private:
    // 当前线程的索引
    int _curr_index;
    // 线程池中的线程数量
    int _thread_cnt;
    // 这是一个指针数组，线程队列数组的指针，每个线程都有一个独立的线程队列用于接收任务
    thread_queue<queue_msg>** _pool;
//    存储线程池中每个线程的线程ID
    pthread_t* _tids;
};
#endif //DTIMEDB_THREAD_POOL_H
