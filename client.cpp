#include <pthread.h>
#include <iostream>
#include <unistd.h> // 为了使用 sleep 函数
#include "server/include/server_header.h"

/**
* client试运行代码，发送数据给服务端
*/

void buz(const char* data, uint32_t len, int cmdid, net_commu* commu, void* usr_data)
{
    std::cout << "server: " << data << std::endl;
    std::string userInput;
    std::cout << "Enter message to send: ";
    std::getline(std::cin, userInput);
    commu->send_data(userInput.c_str(), userInput.size(), cmdid); // 发送用户输入的消息给服务器

}

void whenConnectDone(tcp_client* client, void* args)
{
    std::string reqStr = "hello";;
    client->send_data(reqStr.c_str(), strlen("hello"), 1);
    std::cout << "Connected to server!" << std::endl;
}

void* domain(void* args)
{
    event_loop loop;
    tcp_client client(&loop, "127.0.0.1", 12315);

    client.add_msg_cb(1, buz, nullptr);
    client.onConnection(whenConnectDone);

    loop.process_evs();

    return NULL;
}

int main()
{
    int threadNum = 1;
    pthread_t* tids = new pthread_t[threadNum]; // 创建线程数组
    // 创建多个客户端线程
    for (int i = 0; i < threadNum; ++i)
        pthread_create(&tids[i], NULL, domain, NULL);
    // 等待线程结束
    for (int i = 0; i < threadNum; ++i)
        pthread_join(tids[i], NULL);
    // 释放内存
    delete[] tids;
    return 0;
}