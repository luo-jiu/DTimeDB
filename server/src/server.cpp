#include "boost/asio.hpp"
#include <sys/socket.h>
#include "iostream"
#include "server/include/server.h"
/**
 * 初始化服务器端口
 * @return
 */
 unsigned short port_num= 1110;

int create_server_socket(){
    //创建通信上下文
    boost::asio::io_service io_server;
    boost::asio::ip::tcp proto=boost::asio::ip::tcp::v4();
    boost::asio::ip::tcp::acceptor acceptor(io_server);
    boost::system::error_code ec;
    acceptor.open(proto,ec);
    if (ec.value()!=0){
        std::cout<< "Failed to open the acceptor socket! Error code = "
                << ec.value() << ". Message: " << ec.message();
        return ec.value();
    }
    return 0;
}
//监听端口，接收链接
bool  server_socket_bind(){
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v6::any(),port_num);
    boost::asio::io_service io;
    boost::asio::ip::tcp::acceptor acc(io,ep.protocol());
    boost::system::error_code error;
    acc.bind(ep,error);
    if (error.value()!=0) {
        std::cout
                <<"Failed to Bind Socket ==== Error code: "
                <<error.value()<<",Message: "<<error.message();
        return false;
    }
    return true;
}
//接受客户端连接
bool accept_new_connection(){
    const int BACK_LOG_SIZE= 30;
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::any(),port_num);
    boost::asio::io_service ios;
    try {
        boost::asio::ip::tcp::acceptor acceptor(ios,ep.protocol());
        acceptor.bind(ep);
        acceptor.listen(BACK_LOG_SIZE);
        boost::asio::ip::tcp::socket sock(ios);
        acceptor.accept(sock);
    }catch(boost::system::system_error &error) {
        std::cout << "Error occured! Error code = " << error.code()
                  << ". Message: " << error.what();
        return error.code().value();
    }
}

//关闭连接
//bool server_init(){
//    int _socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
//    //bind
//    sockaddr_in _addr={};
//    _addr.sin_family=AF_INET;
//    _addr.sin_port= htons(1110);
//    _addr.sin_addr=INADDR_ANY;
//    bind(_socket,(sockaddr*)&_addr, sizeof(_addr));
//}