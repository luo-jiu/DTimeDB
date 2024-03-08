//
// Created by illumwang on 24-3-4.
//
#include <fcntl.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include "../include/server_header.h"
//读回调函数
static void tcp_rcb(event_loop* eventLoop,int fd,void *args){
    tcp_conn* conn = (tcp_conn*)args;
    conn->handle_read();
}
//写回调函数
static void tcp_wcb(event_loop* eventLoop,int fd,void *args){
    tcp_conn* conn = (tcp_conn*)args;
    conn->handle_write();
}
//初始化
void tcp_conn::init(int connfd, event_loop *loop) {
    _connfd = connfd;
    _loop = loop;
    // 设置连接描述符为非阻塞模式
//    获取连接描述符当前的文件状态标志。
    int flag = ::fcntl(_connfd,F_GETFL,0);
    ::fcntl(_connfd,F_SETFL,O_NONBLOCK | flag);
    int opend = 1;
    int ret = ::setsockopt(_connfd,IPPROTO_TCP, TCP_NODELAY,&opend, sizeof(opend));
    error_if(ret < 0,"setsockopt TCP_NODELAY");\
    // 调用用户设置的连接建立后回调函数，主要用于初始化连接参数

}

void tcp_conn::handle_read()
{
    // 读取数据到输入缓冲区
    int ret = ibuf.read_data(_connfd);
    if (ret == -1)
    {
//         读取数据错误，记录错误日志，关闭连接并返回
        error_log("read data from socket err");
        clean_conn();
        return;
    } else if (ret == 0 ){
        // 对端关闭连接，记录信息日志，关闭连接并返回
        info_log("connection closed by peer");
        clean_conn();
        return ;
    }
    // 定义通讯头部变量
    commu_head head;
    // 当输入缓冲区中的数据长度大于等于通讯头部长度时，循环处理数据
    while (ibuf.length() >= COMMU_HEAD_LENGTH){
        // 从输入缓冲区中拷贝通讯头部数据到 head 变量
        ::memcpy(&head,ibuf.data(),COMMU_HEAD_LENGTH);
//        通讯头部的长度不合法
        if (head.length >   MSG_LENGTH_LIMIT   ||    head.length<0){
            // 记录错误日志
            error_log("data format error in data head, close connection");
            // 关闭连接并跳出循环
            clean_conn();
            break;
        }
        // 如果输入缓冲区中的数据长度小于完整消息的长度（包含通讯头部长度）
        if (ibuf.length() < COMMU_HEAD_LENGTH + head.length)
        {
            // 当前消息为半包，跳出循环
            break;
        }
        // 在消息调度器中查找是否存在对应的消息处理回调函数
        if (!tcp_server::dispatcher.is_exist(head.cmdid)){
            // 如果没有对应的回调函数，记录错误日志
            error_log("this message has no corresponding callback, close connection");
            // 关闭连接并跳出循环
            clean_conn();
            break;
        }
        // 从输入缓冲区中弹出通讯头部数据
        ibuf.pop(COMMU_HEAD_LENGTH);
        tcp_server::dispatcher.cb(ibuf.data(),head.length,head.cmdid,this);
        ibuf.pop(head.length);
    }
    ibuf.adjust();
}

void tcp_conn::handle_write()
{
    //循环写
    while (obuf.length()){
        int ret = obuf.write_fd(_connfd);
        if (ret == -1){
            // 写入TCP缓冲区错误，记录错误日志，关闭连接并返回
            error_log("write TCP buffer error, close connection");
            clean_conn();
            return;
        }
        if (ret == 0){
            //不是错误，仅返回为0表示此时不可继续写
            break;
        }
    }
    // 如果输出缓冲区为空，则从事件循环中移除写入事件监听
    if (!obuf.length()){
        _loop->del_ioev(_connfd,EPOLLOUT);
    }
}

int tcp_conn::send_data(const char *data, int datalen, int cmdid)
{
//    确定是否有监听
    bool need_listen = false;
//    如果输出缓冲区为空，将标志位设置为true
    if(!obuf.length()){
        need_listen = true;
    }
    commu_head head;
    head.cmdid = cmdid;
    head.length = datalen;
//    写入头部，向缓冲区发送数据
    int ret = obuf.send_data((const char*) &head,COMMU_HEAD_LENGTH);
    if(ret != 0){
        return -1;
    }
//    写入数据
    ret = obuf.send_data(data,datalen);
    if (ret != 0){
//        写入失败将之前写入的头部都要弹出
        obuf.pop(COMMU_HEAD_LENGTH);
        return -1;
    }
//    如果是第一次发送数据的话，需要注册EPOLLOUT事件，确保在套接字可写时再发送数据，避免数据因为套接字缓冲区已满而被阻塞
    if (need_listen){
        _loop->add_ioev(_connfd,tcp_wcb,EPOLLOUT, this);
    }
    return 0;

}
//清理tcp连接
void tcp_conn::clean_conn()
{
//    调用建立连接的回调函数释放参数
    if(tcp_server::connCloseCb){
        tcp_server::connCloseCb(this);
    }
//    删除连接，在服务器中减少连接计数
    tcp_server::dec_conn();
//  删除事件循环
    _loop->del_ioev(_connfd);
    _loop = NULL;
//    清理输入输出缓冲区
    ibuf.clear();
    obuf.clear();
    int fd = _connfd;
    // 使连接文件描述符无效
    _connfd = -1;
    ::close(fd);
}