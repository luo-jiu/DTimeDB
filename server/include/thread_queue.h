/**
 * 这里是一个消息队列
 */
#ifndef DTIMEDB_THREAD_QUEUE_H
#define DTIMEDB_THREAD_QUEUE_H
#include "server_header.h"
#include <queue>
#include <sys/eventfd.h>
#include <unistd.h>

template <typename T>
class thread_queue{
public:
    thread_queue():_loop(NULL){
        // 初始化互斥锁和事件描述符
        ::pthread_mutex_init(&_mutex, NULL);
        _evfd = ::eventfd(0, EFD_NONBLOCK);
        if (_evfd == -1)
        {
            perror("eventfd(0, EFD_NONBLOCK)");
            ::exit(1);
        }
    }
    ~thread_queue(){
        // 销毁互斥锁和关闭事件描述符
        ::pthread_mutex_destroy(&_mutex);
        ::close(_evfd);
    }
    // 发送消息到队列
    void send_msg(const T& item){
//触发事件，通知接收者信息到达
        unsigned  long long number = 1;
        ::pthread_mutex_lock(&_mutex);
        _queue.push(item);
        //使用 write 函数将值为 number 的 unsigned long long 类型数据写入事件描述符 _evfd。
        // 这个写操作是为了触发事件，通知接收方有新消息到达。
        int ret = ::write(_evfd,&number, sizeof(unsigned  long long));
        if (ret == -1)
            perror("eventfd write");
        ::pthread_mutex_unlock(& _mutex);
    }
    // 从队列接收消息
    void recv_msg(std::queue<T>& tmp_queue){
//        用于从事件描述符中读取通知
        unsigned  long long number;
        ::pthread_mutex_lock(&_mutex);
        int ret = ::read(_evfd,&number,sizeof(unsigned long long));
        if (ret == -1)
            perror("eventfd read");
//        将所有已接收的消息移动到临时队列中,在不持有互斥锁的情况下处理消息
        std::swap(tmp_queue, _queue);
        ::pthread_mutex_unlock(& _mutex);
    }
    event_loop* get_loop(){return _loop;}
    // 设置事件循环并安装消息到达事件的回调函数
    void set_loop(event_loop* loop,io_call_back* proc,void *args = NULL){
        _loop = loop;
        _loop->add_ioev(_evfd,proc,EPOLLIN,args);
    }
private:
    int _evfd;
    event_loop*  _loop;
    // 消息队列
    std::queue<T> _queue;
    // 互斥锁
    pthread_mutex_t  _mutex;
};
#endif //DTIMEDB_THREAD_QUEUE_H
