/**
 *数据包结构的封装、定义与处理
 */
#include <string>
#ifndef DTIMEDB_PACKAGE_H
#define DTIMEDB_PACKAGE_H

#define MAX_PACKAGE_LENGTH (256L*256L*256L-1)
typedef int db_socket;
enum dtimedb_status{
    DB_STATUS_CONNECTED,                             /**建立链接成功**/
    DB_STATUS_GET_RESULT,                            /** 查询结果集**/
    DB_STATUS_USE_RESULT,                            /**使用服务端返回的结果集**/
    DB_STATUS_STATEMENT_RESULT,              /**正在获取结果**/
};
//服务器状态
enum Server_Status{

};
enum Client_Status{
    
};
//命令列表
enum Command{
    COM_CONN,
    COM_QUIT,
    COM_QUERY,
    COM_UPDATE,
    COM_INIT_DB,
    COM_CREATE_DB,
    COM_DROP_DB,
};
//网络报文
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
    //网络套接子
    db_socket fd;
    bool  localhost;  //是否来自localhost
    sockaddr_in6 socket;
    size_t  addr_len;
    char  *desc[1024];//连接的描述字符串
    bool is_closed;   //连接是否关闭
    char *read_buffer;
    char *read_pos; //读取数据的指针
    char *read_end;//数据的结束位置
    int    read_timeout;    //读超时
    int    write_timeout;   //写超时
    /**
     * nio操作函数
     */
    void (*nio_delete)(dtimedb_nio*);
    size_t (*nio_read)(dtimedb_nio*,unsigned char *,size_t);
    size_t (*nio_write)(dtimedb_nio*,const unsigned char*,size_t);
}Nio;
typedef struct db_net{


}Net;

#endif //DTIMEDB_PACKAGE_H
