/**
 *设定整体的服务端链接端口和客户端与服务端之间的网络信息封包过程
 */
#include <netinet/in.h>
#include "tcp_conn.h"
#include "thread_pool.h"



class tcp_server{
public:
    //创建服务端
    tcp_server(event_loop* loop,const char * ip,unsigned short port);
    ~tcp_server();
    //服务器保持连接
    void keep_alive(){_keepalive= true; }
//    接受客户端连接
    void do_accept();
//    消息回调函数，当事件循环触发相应事件时，唤醒事件处理器处理消息
    void add_msg_cb(int cmdid,msg_callback* msg_cb,void *usr_data = NULL){ dispatcher.add_msg_cb(cmdid,msg_cb,usr_data); }
    static void inc_conn();
    static void get_conn_num(int &cnt);
    static void dec_conn();
//     返回事件循环对象
    event_loop* loop(){ return _loop; }
//     返回线程池对象
    thread_pool* threadPool() { return _thd_pool; }

private:
//    套接字
    int _sock_fd;
    int _reserve_fd;
    event_loop* _loop;
//    调用线程池
    thread_pool* _thd_pool;
//  连接地址
    struct sockaddr_in _conn_addr;
    socklen_t _sock_len;
//  标志位，是否开启
    bool _keepalive;
//    当前连接数量
    static int _conn_size;
//    最大连接数量
    static int _max_conns;
//    当前连接计数
    static int _curr_conns;
    static pthread_mutex_t _mutex;
public:
//    消息调度器
    static msg_dispatcher dispatcher;
//   连接的数组，服务器维护的所有连接在这里存储
    static tcp_conn** conns;
//    连接回调函数指针
    typedef void (*conn_callback)(net_commu* com);
//    建立连接的函数指针
    static conn_callback connBuildCb;
//    关闭连接的函数指针
    static conn_callback connCloseCb;
//    建立连接的回调函数
    static void onConnBuild(conn_callback cb){ connBuildCb = cb; }
//    关闭连接的回调函数
    static void onConnClose(conn_callback cb){ connCloseCb = cb; }

};
