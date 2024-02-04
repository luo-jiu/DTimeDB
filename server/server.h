/**
 *设定整体的服务端链接端口和客户端与服务端之间的网络信息封包过程
 */
#include "connection_pool/dtimedb.h"
class Server{
    //初始化服务器端口
    bool socket_init();
//发送消息给客户端
    void sned_runnable( );
//读取客户端消息
    void read_runnable();

};
