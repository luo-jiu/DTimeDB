//
// Created by illumwang on 24-3-4.
//

#ifndef DTIMEDB_TCP_CLIENT_H
#define DTIMEDB_TCP_CLIENT_H

#include <csignal>
#include <netinet/in.h>

class tcp_client:public net_commu{
public:
//    初始化客户端
    tcp_client(event_loop* loop,const char *ip,unsigned short port,const char *name = NULL);
//  连接成功回调函数指针
    typedef void onconn_func(tcp_client* client,void *args);
//    连接失败回调函数指针
    typedef void oncls_func(tcp_client* client,void *args);
//    连接成功回调函数
    void onConnection(onconn_func* func, void *args = NULL)
    {
        _onconnection = func;
        _onconn_args = args;
    }

//      连接关闭回调函数
    void onClose(oncls_func* func, void *args = NULL)
    {
        _onclose = func;
        _onclose_args = args;
    }
//  调用回调函数
    void call_onconnect()
    {
        if (_onconnection)
            _onconnection(this, _onconn_args);
    }

    void call_onclose()
    {
        if (_onclose)
            _onclose(this, _onclose_args);
    }
//消息处理回调函数
    void add_msg_cb(int cmdid, msg_callback* msg_cb, void* usr_data = NULL) { _dispatcher.add_msg_cb(cmdid, msg_cb, usr_data); }

    void do_connect();

    virtual int send_data(const char* data, int datlen, int cmdid);

    virtual int get_fd() { return _sock_fd; }

    int handle_read();

    int handle_write();

    ~tcp_client() { ::close(_sock_fd); }

    void clean_conn();

    event_loop* loop() { return eventLoop; }
// 网络是否正常
    bool net_ok;
    io_buffer ibuf, obuf;
    struct sockaddr_in servaddr;
private:
    int  _sock_fd;
    event_loop* eventLoop;
    socklen_t _addr_len;
//    消息分发器
    msg_dispatcher _dispatcher;
//    当连接建立成功时，调用回调函数处理连接任务
    onconn_func* _onconnection;
    void* _onconn_args;
//    关闭连接，当关闭连接时，嗲用回调函数处理
    oncls_func* _onclose;
    void* _onclose_args;
//    客户端代号
    const char* _name;
};
#endif //DTIMEDB_TCP_CLIENT_H
