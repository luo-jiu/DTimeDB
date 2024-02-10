/**
 *设定整体的服务端链接端口和客户端与服务端之间的网络信息封包过程
 */
#include "connection_pool/dtimedb.h"
#include "server/include/event_loop.h"
class Server{
    Server(const char * ip,unsigned short port);
    ~Server();
    void keep_alive(){_keepalive= true; }
    void do_accept();

private:
    int _sock_fd;
    int _reserve_fd;
    struct sockaddr_in _conn_addr;
    socklen_t _add_len;
    bool _keepalive;
    static int _conn_size;
    static int _max_conns;
    static int _curr_conns;
    static pthread_mutex_t _mutex;
public:

};
