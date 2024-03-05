/**
 *设定整体的服务端链接端口和客户端与服务端之间的网络信息封包过程
 */
#include "../include/server_header.h"


class tcp_server{
    //创建客户端
    tcp_server(event_loop* loop,const char * ip,unsigned short port);
    ~tcp_server();
    //服务器保持连接
    void keep_alive(){_keepalive= true; }
    void do_accept();

private:
    int _sock_fd;
    int _reserve_fd;
    event_loop* _loop;

    bool _keepalive;
    static int _conn_size;
    static int _max_conns;
    static int _curr_conns;
    static pthread_mutex_t _mutex;
public:
    // 消息调度器
    static msg_dispatcher dispatcher;
};
