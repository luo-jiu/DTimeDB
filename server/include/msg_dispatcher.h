//
// Created by illumwang on 24-2-10.
//

#ifndef DTIMEDB_MSG_DISPATCHER_H
#define DTIMEDB_MSG_DISPATCHER_H
#include <ext/hash_map>
#include <cassert>
#include "net_commu.h"
#include "msg_head.h"

typedef void msg_callback(const char*data, uint32_t len, int cmdid, net_commu *commu,void * usr_data);
class msg_dispatcher{
public:
    msg_dispatcher(){}
//    添加消息处理
    int add_msg_cb(int cmdid,msg_callback* msg_cb,void *usr_data){
        // 检查是否已经存在相同命令ID的处理函数
        if (_dispatcher.find(cmdid) != _dispatcher.end())
            return -1;
        // 将消息处理函数与命令ID关联起来
        _dispatcher[cmdid] = msg_cb;
        // 将用户数据与命令ID关联起来
        _args[cmdid] = usr_data;
        return 0; // 返回成功
    }
//  判断该指令的处理函数是否存在
    bool is_exist(int cmdid) const{
//        找的着是真，否则是假的
        return _dispatcher.find(cmdid) != _dispatcher.end();
    }
//    调用消息处理函数
    void cb(const char* data, uint32_t len, int cmdid,net_commu* commu)
    {
        assert(is_exist(cmdid));
        msg_callback* func = _dispatcher[cmdid];
        void* usr_data = _args[cmdid];
        func(data, len, cmdid, commu, usr_data);
    }
private:
    __gnu_cxx::hash_map<int ,msg_callback*> _dispatcher;
    __gnu_cxx::hash_map<int,void*> _args;
};
#endif //DTIMEDB_MSG_DISPATCHER_H
