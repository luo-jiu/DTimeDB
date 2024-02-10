//
// Created by illumwang on 24-2-10.
//

#ifndef DTIMEDB_NET_COMMU_H
#define DTIMEDB_NET_COMMU_H
typedef int db_socket;
typedef struct db_vio{
    bool _is_localhost;

}Vio;
typedef struct db_net{
    Vio *vio;
    char  *buff,*buff_end,*write_pos,*read_pos;
    db_socket fd;

}Net;
class net_commu{
public:
    net_commu():_parameter(nullptr){}
    virtual int send_data(const char *data,int data_len,int cmd_id)=0;
    virtual int get_fd()=0;
    void *_parameter;
};
#endif //DTIMEDB_NET_COMMU_H
