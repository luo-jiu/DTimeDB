#include "boost/asio.hpp"
#include <sys/socket.h>
#include "iostream"
/**
 * 初始化服务器端口
 * @return
 */
 unsigned short port_num=1110;
// socket();
int client_bind(){
     boost::asio::ip::address ip_address=boost::asio::ip::address_v6::any();
     boost::asio::ip::tcp::endpoint point(ip_address,port_num);
    return 0;
}

bool socket_init(){
/**
* 创建socket
*/
    //通信上下文
    boost::asio::io_service io;
    boost::asio::ip::tcp proto=boost::asio::ip::tcp::v6();
    boost::asio::ip::tcp::acceptor acceptor(io,boost::asio::ip::tcp::endpoint (proto,port_num));
    //错误码
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