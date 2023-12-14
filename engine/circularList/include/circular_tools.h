//
// Created by illumwang on 12/5/23.
//

#ifndef DTIMEDB_TOOLS_H
#define DTIMEDB_TOOLS_H
#include "chrono"
using namespace std;

/**
 * 时间戳数据类型，默认导入时间戳，目前精度为秒，可以升级为纳秒
 */
class Timestamp{
private:
    time_t timestamp;
public:
    Timestamp() : timestamp(chrono::system_clock::to_time_t(chrono::system_clock::now())){}
    time_t getTimestamp() const{
        return timestamp;
    }
};
#endif //DTIMEDB_TOOLS_H
