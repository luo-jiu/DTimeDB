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

enum Page_TYPE{
    INDEX,
    DATA,
    LOG
};
//
enum TOKEN{
    STATUS,
};
#endif //DTIMEDB_DATA_DEF_H
