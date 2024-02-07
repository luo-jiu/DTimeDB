/**
*这里是dtimedb基础io结构库
 */
#pragma once
#include <stddef.h>
#include <arpa/inet.h>
#include "server/package.h"
/**数据库链接对象**/
typedef struct dtime_db{
    Net                         *net;         /**网络io指针**/
    char                          *host,*unix_socket,*host_info,*user,*password;        /**连接的主机、用户名、密码、Unix socket 路径**/
    char                           *db,*info;           /**数据库的信息**/
    DT_FIELD                *fields;        /**传递字 段信息**/
    unsigned int                              port;
    bool                            free;           /**是否需要释放链接**/
    enum dtimedb_status    status;
}DTIMEDB;
//初始化连接环境
DTIMEDB *conn_init(DTIMEDB *dtimedb);

