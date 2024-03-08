//
// Created by illumwang on 24-3-4.
//

#ifndef DTIMEDB_TCP_CLIENT_H
#define DTIMEDB_TCP_CLIENT_H

#include <csignal>
#include <netinet/in.h>

class tcp_client:public net_commu{
public:
    tcp_client(event_loop* loop,const char *ip,unsigned short port,const char *name = NULL);
    typedef void onconn_func(tcp_client* client,void *args);
    typedef void oncls_func(tcp_client* client,void *args);

    //set up function after connection ok
    void onConnection(onconn_func* func, void *args = NULL)
    {
        _onconnection = func;
        _onconn_args = args;
    }

    //set up function after connection closed
    void onClose(oncls_func* func, void *args = NULL)
    {
        _onclose = func;
        _onclose_args = args;
    }

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

    void add_msg_cb(int cmdid, msg_callback* msg_cb, void* usr_data = NULL) { _dispatcher.add_msg_cb(cmdid, msg_cb, usr_data); }

    void do_connect();

    virtual int send_data(const char* data, int datlen, int cmdid);

    virtual int get_fd() { return _sock_fd; }

    int handle_read();

    int handle_write();

    ~tcp_client() { ::close(_sock_fd); }

    void clean_conn();

    event_loop* loop() { return eventLoop; }

    bool net_ok;
    io_buffer ibuf, obuf;
    struct sockaddr_in servaddr;
private:
    int  _sock_fd;
    event_loop* eventLoop;
    socklen_t _addr_len;
    msg_dispatcher _dispatcher;
//    当连接建立成功时
    onconn_func* _onconnection;
    void* _onconn_args;
//    关闭连接
    oncls_func* _onclose;
    void* _onclose_args;
    const char* _name;
};
#endif //DTIMEDB_TCP_CLIENT_H
