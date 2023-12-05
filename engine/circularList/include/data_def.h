//
// Created by illumwang on 11/29/23.
//

#ifndef DTIMEDB_DATA_DEF_H
#define DTIMEDB_DATA_DEF_H
enum class DataType{
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

enum PageType{
    INDEX,
    DATA,
    LOG
};
//
enum Token{
    STATUS,
};
#endif //DTIMEDB_DATA_DEF_H
