#include <fcntl.h>
#include <arpa/inet.h>
#include "../include/server_header.h"

//接受连接回调函数
void acceptor_cb(event_loop* loop,int fd,void *args)
{
    tcp_server* server = (tcp_server*)args;
    server->do_accept();
}

//初始化
tcp_conn** tcp_server::conns = NULL;
int tcp_server::_conn_size = 0;
int tcp_server::_curr_conns = 0;
int tcp_server::_max_conns = 0;
msg_dispatcher tcp_server::dispatcher;
pthread_mutex_t tcp_server::_mutex;

//用户设置连接释放后的回调函数
tcp_server::conn_callback tcp_server::connCloseCb = NULL;
//用户设置连接建立后的回调函数
tcp_server::conn_callback tcp_server::connBuildCb = NULL;

//初始化对象
tcp_server::tcp_server(event_loop *loop, const char *ip, unsigned short port) {
//    初始化网络地址与信号量

    ::bzero(&_conn_addr, sizeof(_conn_addr));
    if (::signal(SIGHUP, SIG_IGN) == SIG_ERR) {
        error_log("signal ignore SIGHUP");
    }
    if (::signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        error_log("signal ignore SIGPIPE");
    }
//    初始化socket
    _sock_fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    exit_if(_sock_fd == -1, "socket()");
//    创建一个占位的文件描述符，防止文件描述符用尽
    _reserve_fd = ::open("/home/illumwang/reactor_accepter", O_CREAT | O_RDONLY | O_CLOEXEC, 0666);
    error_if(_reserve_fd == -1, "open()");
    struct sockaddr_in servaddr;
    ::bzero(&servaddr,sizeof (servaddr));
    servaddr.sin_family = AF_INET;
//
    int ret = ::inet_aton(ip, &servaddr.sin_addr);;
    exit_if(ret == 0,"ip format %s", ip);
    servaddr.sin_port = htons(port);

//
    int opend = 1;
    ret = ::setsockopt(_sock_fd,SOL_SOCKET,SO_REUSEADDR,&opend,sizeof(opend));
    error_if(ret < 0, "setsockopt SO_REUSEADDR");

//    绑定端口
    ret = bind(_sock_fd,(const struct sockaddr*)&servaddr,sizeof servaddr);
    exit_if(ret == -1, "bind()");
//    监听
    ret = listen(_sock_fd,500);
    exit_if(ret == -1, "listen()");

    info_log("server on %s:%u is running...", ip, port);
//    初始化事件循环
    _loop = loop;
    _sock_len = sizeof(struct sockaddr_in);
//    如果是多线程reactor，创建线程池
    int thread_cnt = config_reader::ins()->GetNumber("reactor", "threadNum", 0);
    _thd_pool = NULL;
    if (thread_cnt)
    {
        _thd_pool = new thread_pool(thread_cnt);
        exit_if(_thd_pool == NULL, "new thread_pool");
    }
//    创建线程池
    _max_conns = config_reader::ins()->GetNumber("reactor", "maxConns", 10000);
    int next_fd = ::dup(1);
    _conn_size = _max_conns + next_fd;
    ::close(next_fd);
    conns = new tcp_conn * [_conn_size];
    exit_if(conns == NULL, "new conns[%d]", _conn_size);
    for (int i = 0; i < _max_conns + next_fd; ++i) {
        conns[i] = NULL;

    }
//        添加接受事件
    _loop->add_ioev(_sock_fd, acceptor_cb, EPOLLIN, this);
}
tcp_server::~tcp_server() {
    _loop->del_ioev(_sock_fd);
    ::close(_sock_fd);
    ::close(_reserve_fd);
}

//接受连接
void tcp_server::do_accept() {
    int connfd;
    bool conn_full = false;
    while (true){
        connfd = ::accept(_sock_fd,(struct sockaddr*)& _conn_addr,&_sock_len);
        if (connfd == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
//            当文件描述符用尽时
            else if (errno == EMFILE)
            {
                conn_full = true;
                ::close(_reserve_fd);
            }
            else if (errno == EAGAIN)
            {
                break;
            }
            else
            {
                exit_log("accept()");
            }
        }
        else if (conn_full)
        {
            ::close(connfd);
            _reserve_fd = ::open("/home/illumwang/reactor_accepter", O_CREAT | O_RDONLY | O_CLOEXEC, 0666);
            error_if(_reserve_fd == -1, "open()");
        }
        else
        {
            int curr_conns;
            get_conn_num(curr_conns);
//            线程数超限
            if (curr_conns >= _max_conns)
            {
                error_log("connection exceeds the maximum connection count %d", _max_conns);
                ::close(connfd);
            }
            else
            {
                assert(connfd < _conn_size);
                if (_keepalive)
                {
                    int opend = 1;
                    int ret = ::setsockopt(connfd,SOL_SOCKET,SO_KEEPALIVE,&opend, sizeof(opend));
                    error_if(ret < 0, "setsockopt SO_KEEPALIVE");
                }
                if (_thd_pool){
                    thread_queue<queue_msg>* queue = _thd_pool->get_next_thread();
                    queue_msg msg;
                    msg.cmd_type = queue_msg::NEW_CONN;
                    msg.connfd = connfd;
                    queue->send_msg(msg);
                } else{
                    tcp_conn* conn = conns[connfd];
                    if (conn){
                        conn->init(connfd,_loop);
                        exit_if(conn == NULL, "new tcp_conn");
                        conns[connfd] = conn;
                    }
                }
            }
        }
    }
}

//增加连接
void tcp_server::inc_conn()
{
    ::pthread_mutex_lock(&_mutex);
    _curr_conns++;
    ::pthread_mutex_unlock(&_mutex);
}

//
void tcp_server::get_conn_num(int &cnt)
{
    ::pthread_mutex_lock(&_mutex);
    cnt = _curr_conns;
    ::pthread_mutex_unlock(&_mutex);
}

//删除连接
void tcp_server::dec_conn()
{
    ::pthread_mutex_lock(&_mutex);
    _curr_conns--;
    ::pthread_mutex_unlock(&_mutex);
}