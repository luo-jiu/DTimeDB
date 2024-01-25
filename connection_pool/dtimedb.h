#pragma once
typedef int my_socket;
#ifndef DTIMEDB_DTIMEDB_H
#define DTIMEDB_DTIMEDB_H
enum dtimedb_status{
    DB_STATUS_CONNECTED,                             /**建立链接成功**/
    DB_STATUS_GET_RESULT,                            /** 查询结果集**/
    DB_STATUS_USE_RESULT,                            /**使用服务端返回的结果集**/
    DB_STATUS_STATEMENT_RESULT,              /**正在获取结果**/
};
/**
 *查询结果的具体信息
 */
typedef struct dtimedb_field{
    char *name;              /**列的名字**/
    char  *tag;                 /**tag的名字**/
    char *table;               /**表的名字**/
    char *db;                  /**数据库的名字**/
    char  *catalog;          /**不同数据库同名表的目录标志**/
    unsigned long length;
    unsigned int table_length;
    unsigned int db_length;
//    Type            field_type;               /**值的类型，到最后会将值整体的统一，放在根目录下的type文件夹内**/
}DT_FIELD;
//底层网络io
typedef struct dtimedb_nio{
    bool  localhost;  //是否来自localhost

};
typedef struct dtimedb_net{
    my_socket fd;
    unsigned int write_timeout,read_timeout,retry_count;

}NET;
/**数据库链接对象**/
struct conn{
    NET                         *net;         /**网络io指针**/
    char                          *host,*unix_socket,*host_info,*user,*password;        /**连接的主机、用户名、密码、Unix socket 路径**/
    char                           *db,*info;           /**数据库的信息**/
    DT_FIELD                *fields;        /**传递字段信息**/
    int                              port;
    bool                            free;           /**是否需要释放链接**/
    enum dtimedb_status    status;
}DTIMEDB;

#endif //DTIMEDB_DTIMEDB_H
