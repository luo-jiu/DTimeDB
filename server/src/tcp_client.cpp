#include <cerrno>
#include <fcntl.h>
#include <cassert>
#include <csignal>
#include <unistd.h>
#include <cstring>
#include <strings.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../include/server_header.h"

//读回调函数
static void read_cb(event_loop* loop, int fd, void* args)
{
    tcp_client* cli = (tcp_client*)args;
    cli->handle_read();
}

//写回调函数
static void write_cb(event_loop* loop, int fd, void* args)
{
    tcp_client* cli = (tcp_client*)args;
    cli->handle_write();
}

//重连回调函数
static void reconn_cb(event_loop* loop, void* usr_data)
{
    tcp_client* cli = (tcp_client*)usr_data;
    cli->do_connect();
}

//连接回调函数
static void connection_cb(event_loop* loop, int fd, void* args)
{
    tcp_client* cli = (tcp_client*)args;
    loop->del_ioev(fd);
    int result;
    socklen_t result_len = sizeof(result);
    getsockopt(fd, SOL_SOCKET, SO_ERROR, &result, &result_len);
    if (result == 0)
    {
        //connect build success!
        cli->net_ok = true;
        info_log("connect %s:%d successfully", ::inet_ntoa(cli->servaddr.sin_addr), ntohs(cli->servaddr.sin_port));

        //call on connection callback(if has)
        cli->call_onconnect();

        loop->add_ioev(fd, read_cb, EPOLLIN, cli);
        if (cli->obuf.length)
        {
            loop->add_ioev(fd, write_cb, EPOLLOUT, cli);
        }
    }
    else
    {
        //connect build error!
        //reconnection after 2s
        info_log("connect %s:%d error, retry after 2s", ::inet_ntoa(cli->servaddr.sin_addr), ntohs(cli->servaddr.sin_port));
        loop->run_after(reconn_cb, cli, 2);
    }
}

//  初始化客户端
tcp_client::tcp_client(event_loop *loop, const char *ip, unsigned short port, const char *name):
        net_ok(false),
//        初始化输入输出缓冲区的容量
        ibuf(4194304),
        obuf(4194304),
        _sock_fd(-1),
        eventLoop(loop),
        _onconnection(NULL),
        _onconn_args(NULL),
        _onclose(NULL),
        _onclose_args(NULL),
        _name(name)
{
    ::bzero(&servaddr,sizeof (servaddr));
    servaddr.sin_family = AF_INET;
//    将字符串形式的IP地址转换为二进制形式
    int ret = ::inet_aton(ip,&servaddr.sin_addr);
//    检验ip地址是否正确
    exit_if(ret == 0,"ip format %s", ip);
//    设置端口号，并转换为网络字节序
    servaddr.sin_port = htons(port);
    _addr_len = sizeof servaddr;
//    建立连接
    do_connect();
}

//连接
void tcp_client::do_connect() {
    if (_sock_fd != -1)
    {
        ::close(_sock_fd);
//       创建套接字
        _sock_fd = ::socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, IPPROTO_TCP);
        exit_if(_sock_fd == -1, "socket()");
//        连接服务器
        int ret = ::connect(_sock_fd,(const struct sockaddr*)&servaddr, _addr_len);
        if (ret == 0){
//            连接创建成功
            net_ok = true;
            call_onconnect();
            info_log("connect %s:%d successfully", ::inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));
        }
        else
        {
            if (errno == EINPROGRESS)
            {
                eventLoop->add_ioev(_sock_fd,connection_cb,EPOLLOUT,this);
            }
            else
            {
                exit_log("error occur when client connect");
            }
        }
    }
}

int tcp_client::send_data(const char *data, int datlen, int cmdid)
{
//    如果没连接
    if (!net_ok){
        return -1;
    }
//  检查是否需要添加写事件的监听
    bool need = (obuf.length == 0) ? true : false;
    if (datlen + COMMU_HEAD_LENGTH > obuf.capacity - obuf.length)
    {
        error_log(" no more space to write socket");
        return -1;
    }
    commu_head head;
    head.cmdid = cmdid;
    head.length = datlen;
//  将消息头部数据拷贝到输出缓冲区
    ::memcpy(obuf.data + obuf.length, &head, COMMU_HEAD_LENGTH);
    obuf.length += COMMU_HEAD_LENGTH;
//  将数据拷贝到输出缓冲区
    ::memcpy(obuf.data + obuf.length, &head,COMMU_HEAD_LENGTH);
    obuf.length += datlen;
//    如果需要写事件监听，则添加写事件
    if (need)
    {
        eventLoop->add_ioev(_sock_fd,write_cb,EPOLLOUT,this);
    }
    return 0;

}

int tcp_client::handle_read()
{
    assert(net_ok);
    int rn;
//    查询缓冲区可读数据大小
    if (::ioctl(_sock_fd,FIONREAD,&rn) == -1)
    {
        error_log("ioctl FIONREAD");
        return -1;
    }
//    读取数据不超出输入缓冲区容量
    assert(rn <= ibuf.capacity - ibuf.length);
    int ret;
    do {
//        读取数据到输入缓冲区
        ret = ::read(_sock_fd, ibuf.data + ibuf.length, rn);
    } while (ret == -1 && errno == EINTR);
//        对端关闭连接
    if (ret == 0)
    {
        if (_name)
        {
            info_log("%s client: connection closed by peer", _name);
        } else{
            info_log("client: connection closed by peer");
        }
        clean_conn();
        return -1;
    }
//    数据读取错误
    else if (ret == -1)
    {
        assert(errno != EAGAIN);
        error_log("read()");
        clean_conn();
        return -1;
    }
    assert(ret == rn);
    ibuf.length += ret;
    commu_head head;
    int cmdid,length;
    while (ibuf.length >= COMMU_HEAD_LENGTH)
    {
//        处理消息头
        ::memcpy(&head,ibuf.data + ibuf.head,COMMU_HEAD_LENGTH);
        cmdid = head.cmdid;
        length = head.length;
//        如果容量足够，则直接跳出循环去处理数据
        if (length + COMMU_HEAD_LENGTH < ibuf.length)
        {
            break;
        }
//        弹出已经处理过的消息头部
        ibuf.pop(COMMU_HEAD_LENGTH);
//        不存在
        if (!_dispatcher.is_exist(cmdid))
        {
            error_log("this message has no corresponding callback, close connection");
            clean_conn();
            return -1;
        }
        _dispatcher.cb(ibuf.data + ibuf.head,length,cmdid, this);
        ibuf.pop(length);
    }
    ibuf.adjust();
    return 0;
}

//处理可写事件
int tcp_client::handle_write()
{
//    确保输出缓冲区中有数据
    assert(obuf.head == 0 && obuf.length);
    int ret;
    while (obuf.length)
    {
        do {
            ret = ::write(_sock_fd,obuf.data,obuf.length);
        } while (ret == -1 && errno == EINTR);
        if (ret > 0)
        {
            obuf.pop(ret);
            obuf.adjust();
        }
//      如果写入返回值为-1且错误码不是EAGAIN，记录错误并清理连接
        else if (ret == -1 && errno != EAGAIN)
        {
            error_log("write()");
            clean_conn();
            return -1;
        }
//      如果写入返回值为0或者写入失败且错误码为EAGAIN，说明不可继续写入，退出循环
        else{
            break;
        }
    }
//  如果obuf中没有数据，将套接字从写事件监听中移除
    if (!obuf.length)
    {
        eventLoop->del_ioev(_sock_fd,EPOLLOUT);
    }
    return 0;
}
// 清理连接
void tcp_client::clean_conn()
{
    if (_sock_fd != -1){
        eventLoop->del_ioev(_sock_fd);
        ::close(_sock_fd);
    }
    net_ok = false;
    call_onclose();
    do_connect();
}