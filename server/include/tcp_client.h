//
// Created by illumwang on 24-3-4.
//

#ifndef DTIMEDB_TCP_CLIENT_H
#define DTIMEDB_TCP_CLIENT_H

#include <csignal>
class tcp_client:public net_commu{
public:
private:
    int  _sock_fd;
    event_loop* eventLoop;
    socklen_t _addr_len;
    msg_dispatcher _dispatcher;

};
#endif //DTIMEDB_TCP_CLIENT_H
