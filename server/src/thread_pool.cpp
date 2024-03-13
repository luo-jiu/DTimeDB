//
// Created by illumwang on 24-2-18.
//
#include "server/include/server_header.h"
#include "server/include/thread_pool.h"
//消息到达，当消息队列中有数据来的时候，调用这个函数处理消息类型
void msg_comming_cb(event_loop* loop,int fd,void *args){
//    将县城参数转换为队列指针
    thread_queue<queue_msg>* queue = (thread_queue<queue_msg>*)args;
    std::queue<queue_msg> msgs;
    queue->recv_msg(msgs);
    while (!msgs.empty())
    {
        queue_msg msg = msgs.front();
        msgs.pop();
//        处理连接事件
        if (msg.cmd_type == queue_msg::NEW_CONN)
        {
            tcp_conn* conn = tcp_server::conns[msg.connfd];
            if (conn)
            {
             conn->init(msg.connfd,loop);
            }
//            处理任务事件
            else if (msg.cmd_type == queue_msg::NEW_TASK)
            {
                loop->add_task(msg.task,msg.args);
            }
        }
    }
}

//线程入口函数，每个工作线程在启动时执行该函数
void* thread_domain(void* args){
    thread_queue<queue_msg>* queue = (thread_queue<queue_msg>*)args;
    event_loop* loop = new event_loop();
    exit_if(loop == NULL,"new event_loop() in thread_pool");
    queue->set_loop(loop,msg_comming_cb,queue);
    loop->process_evs();
    return NULL;
}

//线程池构造函数
thread_pool::thread_pool(int thread_cnt) :_curr_index(0), _thread_cnt(thread_cnt)
{
    exit_if(thread_cnt <= 0 || thread_cnt > 30,"error thread cnt %d",thread_cnt);
//    创建县城数组和id数组
    _pool = new thread_queue<queue_msg> * [thread_cnt];
    _tids = new pthread_t [thread_cnt];
    int  ret;
//    初始化线程队列
    for (int i = 0; i < thread_cnt; ++i) {
        _pool[i] = new thread_queue<queue_msg>();
        ret = ::pthread_create(&_tids[i],NULL,thread_domain,_pool[i]);
        error_if(ret == -1,"pthread_create");
        ret = ::pthread_detach(_tids[i]);
        error_if(ret == -1,"pthread_detach");
    }
}

// 向线程池中的所有线程队列发送任务
void thread_pool::run_task(pendingFunc task, void *args)
{
    for (int i = 0; i < _thread_cnt; ++i) {
        run_task(i,task);
    }
}

// 向指定线程队列发送任务
void thread_pool::run_task(int thd_index, pendingFunc task, void *args)
{
    queue_msg msg;
    msg.cmd_type = queue_msg::NEW_TASK;
    msg.task = task;
    msg.args = args;
    thread_queue<queue_msg>* cq = _pool[thd_index];
    cq->send_msg(msg);
}

//获取下一个线程队列
thread_queue <queue_msg> *thread_pool::get_next_thread()
{
    if (_curr_index == _thread_cnt)
    {
        _curr_index = 0;
    }
    return _pool[_curr_index++];
}