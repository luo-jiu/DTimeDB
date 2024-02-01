/**
 * 使用懒汉模式维护一个连接池，使用对象池模式管理连接池
 */
#ifndef DTIMEDB_CONNECTION_POOL_H
#define DTIMEDB_CONNECTION_POOL_H
#include "queue"
#include "connection.h"
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
    int max_Idle_conn;
    int min_Idle_conn;
    int  max_connection;
    int  min_connection;
    int  max_time_out;
    bool  is_active;
    std::queue<Connection> conn;
    std::mutex  conn_lock;
};
#endif //DTIMEDB_CONNECTION_POOL_H
