//
// Created by illumwang on 24-2-13.
//
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#ifndef DTIMEDB_PRINT_ERROR_H
#define DTIMEDB_PRINT_ERROR_H
#define error_if(condition, fmt, args...) \
    do { \
        if (condition) { \
            if (errno) { \
                fprintf(stderr, "ERROR: %s " fmt "\n", strerror(errno), ##args); \
            } else { \
                fprintf(stderr, "ERROR:" fmt "\n", ##args); \
            } \
        } \
    } while (0)

#define info_log(fmt, args...) fprintf(stdout, fmt "\n", ##args)

#define error_log(fmt, args...) error_if(1, fmt, ##args)

inline void exit_if(int condition,const char *fmt, ...){
    va_list arg_list;
    if (condition){
        va_start(arg_list,fmt);
        fprintf(stderr,fmt,arg_list);
        if (errno){
            fprintf(stderr,": %s",strerror(errno));
        }
        fprintf(stderr,"\n");
        va_end(arg_list);
        exit(EXIT_FAILURE);
    }
}
#define exit_log(fmt,args...) exit_if(1,fmt,##args)
#endif //DTIMEDB_PRINT_ERROR_H
