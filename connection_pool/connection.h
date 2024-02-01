/**
 * 这是一个数据库的链接类，在这里将完成对数据库客户端与服务端之间的链接格式定义
 */
#ifndef DTIMEDB_CONNECTION_H
#define DTIMEDB_CONNECTION_H
#include "dtimedb.h"
#include <string>

class Connection{
    //初始化数据库链接
    Connection();
    //释放数据库连接
    ~Connection();
    //链接数据库
    /**没有加上用户权限管理**/
    //bool connect(std::string db_name,std::string ip,std::string user,std::string password,unsigned int port=1110);
    bool connect(std::string db_name,std::string ip,std::string user,std::string password,unsigned int port=1110);
    //更新数据库
    bool update_db(std::string sql);
    //查询数据库
    bool query_db(const char *str);
    //遍历查询获得的结果集
    bool next();
    //查询返回字段
    string value(int index);
private:
    DTIMEDB *dtimedb= nullptr;
    DT_FIELD  *field= nullptr;
    //查询结果的api
    //1. tsm

    //2. clt

};
#endif //DTIMEDB_CONNECTION_H
