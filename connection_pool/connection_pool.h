/**
 * 使用懒汉模式维护一个连接池
 */
#ifndef DTIMEDB_CONNECTION_POOL_H
#define DTIMEDB_CONNECTION_POOL_H
using namespace std;
#include "queue"
class ConnectionPool{
public:
    static ConnectionPool *getConnectionPool();
    ConnectionPool (const ConnectionPool &obj)=delete;
private:
    ConnectionPool();
    queue<Connection>
};
#endif //DTIMEDB_CONNECTION_POOL_H
