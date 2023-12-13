//
// Created by illumwang on 11/29/23.
//

#ifndef DTIMEDB_DATA_DEF_H
#define DTIMEDB_DATA_DEF_H
enum class DATA_TYPE{
    INTEGER,
    DOUBLE,
    STRING,
    VARCHAR,
    BYTE=0,
    TEXT,
    FLOAT,
    LONG,
    CHAR,
};
//页类型
enum class PAGE_TYPE{
    INDEX,
    DATA,
    LOG
};
//信息传输标识
enum TOKEN{
    STATUS,
};
#endif //DTIMEDB_DATA_DEF_H
