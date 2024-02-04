#include "boost/asio.hpp"
#include <sys/socket.h>
#include "iostream"
#include "server.h"
/**
 * 初始化服务器端口
 * @return
 */
 unsigned short port_num= htons(1110);
// socket();
int client_bind(){
     boost::asio::ip::address ip_address=boost::asio::ip::address_v6::any();
     boost::asio::ip::tcp::endpoint point(ip_address,port_num);
    return 0;
}

bool Server::socket_init(){
    //创建通信上下文
    boost::asio::io_service io;
    boost::asio::ip::tcp proto=boost::asio::ip::tcp::v6();
    boost::asio::ip::tcp::acceptor acceptor(io,boost::asio::ip::tcp::endpoint (proto,port_num));
    boost::system::error_code error;
    acceptor.open(proto,error);
    if (error.value()!=0){
        std::cout
        <<"Failed to Create Socket ==== Error code: "
        <<error.value()<<",Message: "<<error.message();
        return false;
    }
    return true;
}
//监听端口，接收链接
bool  socket_bind(){
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v6::any(),port_num);
    boost::asio::io_service io;
    boost::asio::ip::tcp::acceptor acc(io,ep.protocol());
    boost::system::error_code error;
    acc.bind(ep,error);
    if (error.value()!=0){
        std::cout
                <<"Failed to Bind Socket ==== Error code: "
                <<error.value()<<",Message: "<<error.message();
        return false;
    }
    return true;
}

//bool server_init(){
//    int _socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
//    //bind
//    sockaddr_in _addr={};
//    _addr.sin_family=AF_INET;
//    _addr.sin_port= htons(1110);
//    _addr.sin_addr=INADDR_ANY;
//    bind(_socket,(sockaddr*)&_addr, sizeof(_addr));
//}