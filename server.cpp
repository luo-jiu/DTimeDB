#include "server/include/server_header.h"
#include <iostream>

/**
* 数据库服务端demo，客户端直接传字符串过来，接受字符串并调用解析器解析
*/
void printLogo(){
    std::cout <<" ___________________.__               ________ __________   "<< std::endl;
    std::cout <<R"(\______ \__    ___/|__| _____   ____ \______ \\______   \ )"<< std::endl;
    std::cout <<R"( |    |  \|    |   |  |/     \_/ __ \ |    |  \|    |  _/ )"<< std::endl;
    std::cout <<R"( |    `   \    |   |  |  Y Y  \  ___/ |    `   \    |   \)"<< std::endl;
    std::cout <<"/_______  /____|   |__|__|_|  /\\___  >_______  /______  /"<< std::endl;
    std::cout <<R"(        \/                  \/     \/        \/       \/ )"<< std::endl;
    std::cout <<R"( ============== WELCOME TO USE DTIMEDB ! =============== )"<< std::endl;
}
//将接受到的数据发送回去
void buz(const char* data, uint32_t len, int cmdid, net_commu* commu, void* usr_data)
{
//    从客户端接收到的消息
    std::cout << data
    <<"sss"<< std::endl;
//    在这里做解析数据的处理
    std::string answer = " from server";
    std::string rspStr = data + answer;
//    处理结果发送回客户端
    int recv = commu->send_data(rspStr.c_str(), rspStr.size(), cmdid);//回复消息
    if (recv != 0){
        std::cout << "Failed to send response." << std::endl;
    }
}
int main(){
    printLogo();
//单线程服务端
    event_loop loop;
    config_reader::setPath("myconf.ini");
    std::string ip = config_reader::ins()->GetString("reactor", "ip", "0.0.0.0");
    short port = config_reader::ins()->GetNumber("reactor", "port", 12315);

    tcp_server server(&loop, ip.c_str(), port);//创建TCP服务器
    server.add_msg_cb(1, buz);
    loop.process_evs();
    return 0;
}