#include "connection.h"
//调用dtimedb的init函数初始化数据库的连接对象
Connection::Connection(){

}
//释放链接，调用dtimedb的close函数
Connection::~Connection() {

}
//连接数据库
bool Connection::connect(std::string db_name, std::string ip, std::string user, std::string password,
                         unsigned int port) {

}
//更新数据库操作，包括增、删除、改
bool Connection::update_db(std::string sql) {

}
//发送查询命令到服务器
bool Connection::query_db(const char *str) {

}
//