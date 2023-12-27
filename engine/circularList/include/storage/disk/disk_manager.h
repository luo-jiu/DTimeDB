//
// Created by illumwang on 23-12-24.
//

#ifndef DTIMEDB_DISK_MANAGER_H
#define DTIMEDB_DISK_MANAGER_H
#pragma once
#include <mutex>
#include <fstream>
#include "string"
namespace ctl{
    class DiskManager{
    public:
        explicit DiskManager(const std::string &db_file);
        ~DiskManager()=default;
        void WritePage();
        void ReadPage();
        void CreateDatabase(const std::string &db_name);
    private:
        // 获取文件大小
        int GetFileSize(const std::string &file_name);
        //用于写入日志的文件流
        std::fstream                                log_io;
        std::string                                   log_name;
        //用于写入数据表的文件流
        std::fstream                                table_io;
        std::string                                   file_name;
        //对于数据库（文件夹）的操作
        std::string                                   folder_path;
        std::mutex                                  file_io_latch;
    };
}
#endif //DTIMEDB_DISK_MANAGER_H
