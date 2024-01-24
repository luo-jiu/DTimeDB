/**
 * 这是一个数据库的链接类，在这里将完成对数据库客户端与服务端之间的链接格式定义
 */
#ifndef DTIMEDB_CONNECTION_H
#define DTIMEDB_CONNECTION_H

#include <string>

class Connection{
    //初始化数据库链接
    Connection();
    //
    ~Connection();
    //链接数据库
    //更新数据库
    //执行数据库操作
private:
    int                db_port_;
    std::string      db_ip_;

};
#endif //DTIMEDB_CONNECTION_H
