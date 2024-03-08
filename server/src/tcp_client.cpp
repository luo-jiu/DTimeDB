#include <arpa/inet.h>
#include "../include/server_header.h"

static void read_cb(event_loop* loop, int fd, void* args)
{
    tcp_client* cli = (tcp_client*)args;
    cli->handle_read();
}

static void write_cb(event_loop* loop, int fd, void* args)
{
    tcp_client* cli = (tcp_client*)args;
    cli->handle_write();
}

static void reconn_cb(event_loop* loop, void* usr_data)
{
    tcp_client* cli = (tcp_client*)usr_data;
    cli->do_connect();
}

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


tcp_client::tcp_client(event_loop *loop, const char *ip, unsigned short port, const char *name):
        net_ok(false),
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

}


void tcp_client::do_connect() {}

int tcp_client::send_data(const char *data, int datlen, int cmdid) {}

int tcp_client::handle_read() {}

int tcp_client::handle_write() {}

void tcp_client::clean_conn() {}