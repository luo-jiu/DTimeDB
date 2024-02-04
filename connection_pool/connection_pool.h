/**
 * 使用懒汉模式维护一个连接池，使用对象池模式管理连接池
 */
#ifndef DTIMEDB_CONNECTION_POOL_H
#define DTIMEDB_CONNECTION_POOL_H
#include "queue"
#include "connection.h"
#include <condition_variable>
class ConnectionPool{
public:
    //单例模式维护连接池
    static ConnectionPool *getConnectionPool();
    //不允许拷贝构造函数创建与使用
    ConnectionPool (const ConnectionPool &obj)=delete;
    ~ConnectionPool();
    Connection *get_conn();
    void release_conn(Connection *conn);
private:
    //无法通过外部操作创建连接池，保证连接池创建是唯一的
    ConnectionPool();
    //最大空闲线程
//    int max_Idle_conn;
//    //
//    int min_Idle_conn;
//    最大连接数
    int  max_connection;
//    最小连接数
    int  min_connection;
//    最大超时时间，超过这个时间则判定连接关闭，将连接还到连接池
    int  max_time_out;
//    判断连接是否活跃
    bool  is_active;
//    连接队列，用连接出队，还链接入队
    std::queue<Connection> conn;
//     互斥锁，保证一条连接同一时间内只有一个用户
/**
 * 这里是阻塞的，稍后会用异步方法优化
 */
    std::mutex  conn_lock;
//    生产者线程和消费者线程
    std::condition_variable
};
#endif //DTIMEDB_CONNECTION_POOL_H
