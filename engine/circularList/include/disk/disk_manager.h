//
// Created by illumwang on 23-12-24.
//

#ifndef DTIMEDB_DISK_MANAGER_H
#define DTIMEDB_DISK_MANAGER_H
#include "string"
namespace ctl{
    class DiskManager{
    public:
        explicit DiskManager(const std::string &db_file);
        ~DiskManager()=default;
        void WritePage();
    private:

    };
}
#endif //DTIMEDB_DISK_MANAGER_H
