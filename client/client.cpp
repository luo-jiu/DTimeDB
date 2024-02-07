#include "client.h"
#include <boost/asio.hpp>
#include <iostream>
int  create_client_socket(){
    boost::asio::io_service io_client;
    boost::asio::ip::tcp protocol=boost::asio::ip::tcp::v4();
    boost::asio::ip::tcp::socket sock(io_client);
    boost::system::error_code ec;
    sock.open(protocol,ec);
    if (ec.value()!=0){
        std::cout<< "Failed to open the socket! Error code = "
                 << ec.value() << ". Message: " << ec.message();
        return ec.value();
    }
    return 0;
}