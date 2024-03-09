
#include "server/include/server_header.h"
#include <sys/epoll.h>
#include <cassert>
//定时器事件回调函数，处理定时器超时事件
void timer_queue_cb(event_loop* loop,int fd,void *args){
    //存储触发的定时器事件
    std::vector<timer_event> fired_evs;
    //获取超时的定时器事件
    //流程是：
    //对应的事件对象loop
    loop->_timer_que->get_timo(fired_evs);
    //遍历回调函数，依次触发
    for (std::vector<timer_event>::iterator it = fired_evs.begin();
            it != fired_evs.end(); ++it) {
        it->time_cb(loop,it->cb_data);
    }
}
//构造函数的初始化，创建一个事件
event_loop::event_loop() {
    //epoll实例
    _epfd = ::epoll_create1(0);
    //epoll创建错误处理
    exit_if(_epfd==-1,"error happened when epoll_create1()");
    //创建定时器队列
    _timer_que=new timer_queue();
    exit_if(_timer_que== nullptr,"error happened when timer create");
   //将定时器事件注册到事件循环中，是这些事件在特定时间下触发，从而执行相应的任务
   //参数意义：获取定时器队列相关的fd、回调函数、文件描述符可读触发事件的标志、定时器队列指针（对定时器队列的操作）
    add_ioev(_timer_que->notifier(),timer_queue_cb,EPOLLIN,_timer_que);
}

/**
 * 事件处理的主循环函数
 */
void event_loop::process_evs() {
    while (true){
        //处理io事件的迭代器，遍历事件映射列表io_evs
        io_ev_it it;
        //最多等待10毫秒
        int nfds = ::epoll_wait(_epfd,_fired_evs,MAX_EVENTS,10);
        //遍历触发的事件
        for (int i = 0; i < nfds; ++i) {
            //查找事件对应的回调函数
            it = _io_evs.find(_fired_evs[i].data.fd);
            //断言找到的迭代器不为空
            assert(it != _io_evs.end());
            //获取事件的回调函数
            io_event *event = &(it->second);
            //读事件
            /**
             * 参数说明：EPOLLIN——fd可以被读取
	         *  EPOLLOUT——fd可以被写入
	         *  EPOLLHUP——fd被挂起，一般是对端关闭连接
	         *  EPOLLERR——fd错误，对应的套接字或文件错误
             */
            if (_fired_evs[i].events & EPOLLIN){
                //获取事件参数
                void *args = event->read_args;
                event->read_cb(this,_fired_evs[i].data.fd,args);
            }
            //写事件
            else if (_fired_evs[i].events & EPOLLOUT){
                void *args = event->read_args;
                event->write_cb(this,_fired_evs[i].data.fd,args);
            }
            //事件挂起错误
            else if (_fired_evs[i].events & (EPOLLHUP || EPOLLERR)){
                if (event->read_cb){
                    void *args=event->read_args;
                    event->read_cb(this,_fired_evs[i].data.fd, args);
                }
                else if(event->write_cb){
                    void *args=event->write_args;
                    event->write_cb(this, _fired_evs[i].data.fd, args);
                }
                //不存在读写回调
                else{
                    del_ioev(_fired_evs[i].data.fd);
                }
            }
        }
        run_task();
    }
}
//向事件循环中注册事件
void event_loop::add_ioev(int fd, io_call_back *proc, int mask, void *args) {
    //事件掩码
    /**
     * 表示文件描述符关注的事件类型，在 epoll 中通常使用位掩码来表示。具体来说，事件掩码是一个整数值，每个位代表一种事件类型，如 EPOLLIN、EPOLLOUT、EPOLLERR 等
     *
     */
    int f_mask = 0;
    int op;
    //迭代器找到对应事件描述符
    io_ev_it it = _io_evs.find(fd);
    //未找到，则表示新的事件
    if (it == _io_evs.end()){
        f_mask = mask;
        //添加事件
        op = EPOLL_CTL_ADD;
    }
    //找到则修改以有的事件
    else
    {
        f_mask = it->second.mask | mask;
        //修改以有事件
        op = EPOLL_CTL_MOD;
    }
    //读事件
    if ( mask & EPOLLIN){
        //设置读事件回调
        _io_evs[fd].read_cb = proc;
        //设置读事件参数
        _io_evs[fd].read_args = args;
    }
    //写事件
    else{
        _io_evs[fd].write_cb = proc;
        _io_evs[fd].write_args = args;
    }
    //更新事件掩码
    _io_evs[fd].mask = f_mask;
    struct epoll_event event;
    event.events = f_mask;
    event.data.fd = fd;
    int ret = ::epoll_ctl(_epfd, op,fd,&event);
    error_if(ret == -1, "epoll_ctl");
    //加入到监听集合中
    listening.insert(fd);
}
/**
 * 当使用完客户端并且需要关闭客户端连接时，从事件队列中移除对应事件
 * @param fd
 */
//删除事件，完全删除
void event_loop::del_ioev(int fd) {
    _io_evs.erase(fd);
    listening.erase(fd);
    ::epoll_ctl(_epfd, EPOLL_CTL_DEL,fd,nullptr);
}
//删除事件，保留对写事件的监听
void event_loop::del_ioev(int fd, int mask) {
    io_ev_it it = _io_evs.find(fd);
    if (it == _io_evs.end()){
        return;
    }
    //获取当前事件掩码
    int &o_mask = it->second.mask;
    int ret;
    //删除掩码
    o_mask = o_mask & (~mask);
    //掩码已清空
    if (o_mask == 0){
        _io_evs.erase(it);
        ret =  ::epoll_ctl(_epfd,EPOLL_CTL_DEL,fd, nullptr);
        //从监听队列移除
        listening.erase(fd);
    }
    //掩码未清空，说明仍有监听，需要实时更新
    else{
        //创建 epoll 事件结构体
        struct epoll_event event;
        //设置事件类型
        event.events = o_mask;
        event.data.fd = fd;
        ret =  ::epoll_ctl(_epfd,EPOLL_CTL_DEL,fd, &event);
        error_if(ret == -1, "epoll_ctl EPOLL_CTL_MOD");
    }
}
//添加任务
void event_loop::add_task(pendingFunc func, void *args) {
    //创建任务对象
    std::pair<pendingFunc, void*> item(func,args);
    //添加任务对象到任务队列
    _pending_func.push_back(item);
}

//删除定时器事件
void event_loop::del_timer(int timer_id) {
    _timer_que->del_timer(timer_id);
}

//延迟运行定时器事件
int event_loop::run_after(time_call_back *cb, void *args, int sec, int mills) {
    struct timespec tpc;
    //获取当前时间
    clock_gettime(CLOCK_REALTIME, &tpc);
    //转换为毫秒
    uint64_t  ts = tpc.tv_sec * 1000 + tpc.tv_nsec / 1000000UL;
    // 计算延迟时间
    ts += sec * 1000 + mills;
    // 创建定时器事件对象
    timer_event te (cb,args,ts);
    return _timer_que->add_timer(te);
}
//指定时间内运行定时器事件
//定时器队列会不断检查ts并排序，到了时间就会执行
int event_loop::run_at(time_call_back *cb, void *args, uint64_t timestamp) {
    timer_event te(cb,args,timestamp);
    return _timer_que->add_timer(te);
}
//每隔一段事件运行定时器事件
int event_loop::run_every(time_call_back *cb, void *args, int sec, int millis) {
    // 计算间隔时间
    uint32_t interval = sec * 1000 + millis;
    struct timespec tpc;
    //获取当前时间
    clock_gettime(CLOCK_REALTIME,&tpc);
    //下次运行时间
    uint64_t timestamp = tpc.tv_sec * 1000 +  tpc.tv_nsec / 1000000UL + interval;
    timer_event te(cb,args,timestamp,interval);
    return _timer_que->add_timer(te);
}
//运行任务
void event_loop::run_task() {
    //创建迭代器
    std::vector<std::pair<pendingFunc , void*>>::iterator  it;
    for (it = _pending_func.begin(); it != _pending_func.end(); ++ it ) {
        pendingFunc func = it->first;
        void *args = it->second;
        //调用回调函数并且传入当前实例
        func(this, args);
    }
    _pending_func.clear();
}
