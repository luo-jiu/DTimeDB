//
// Created by illumwang on 24-2-10.
//

#ifndef DTIMEDB_TCP_CONN_H
#define DTIMEDB_TCP_CONN_H
#include "../include/server_header.h"
class tcp_conn:public net_commu{
public:
    tcp_conn(int _connfd,event_loop* loop){ init(_connfd,_loop);}
    // 初始化函数，设置连接描述符和事件循环对象
    void init(int _connfd,event_loop* loop);
    // 处理读事件
    void handle_read();
    // 处理写事件
    void handle_write();
    // 发送数据到客户端
    virtual int send_data(const char* data,int datalen,int cmdid);
    // 获取连接的文件描述符
    void clean_conn();
private:
    int _connfd;
    event_loop* _loop;
    input_buffer ibuf;
    output_buffer obuf;
};
#endif //DTIMEDB_TCP_CONN_H
