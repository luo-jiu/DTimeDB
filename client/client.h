//
// Created by illumwang on 24-2-4.
//

#ifndef DTIMEDB_CLIENT_H
#define DTIMEDB_CLIENT_H

#include "server/include/server_header.h"
class client: public net_commu{
public:
    void query_database(const char* query);
private:
    int _sockfd;
    event_loop* _loop;
    socklen_t _adderlen;

};


#endif //DTIMEDB_CLIENT_H
