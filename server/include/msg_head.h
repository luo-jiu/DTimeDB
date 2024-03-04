//
// Created by illumwang on 24-2-10.
//

#ifndef DTIMEDB_MSG_HEAD_H
#define DTIMEDB_MSG_HEAD_H
class event_loop;
enum DB_OPERATION{
    SQL_QUERY,
    SQL_UPDATE,
    SQL_DELETE
};
//通信消息的头部信息，可以防止数据包粘包
struct commu_head{
    int cmdid;
    //协议长度
    int length;
};
// 用于 accepter 与连接通信
// 用于向子线程分发任务
struct queue_msg{
    enum MSG_TYPE{
        // 新连接
        NEW_CONN,
        STOP_THE,
        // 新任务
        NEW_TASK
    };
    MSG_TYPE cmd_type;
    union{
        int connfd;
        struct {
            void (*task)(event_loop*,void *);
            void *args;
        };
    };
};

#define COMMU_HEAD_LENGTH 8
#define MSG_LENGTH_LIMIT (65536 - COMMU_HEAD_LENGTH)
#endif //DTIMEDB_MSG_HEAD_H
