//
// Created by illumwang on 23-12-26.
//

#ifndef DTIMEDB_RWLATCH_H
#define DTIMEDB_RWLATCH_H
#pragma once
#include <mutex>
#include <condition_variable>

namespace ctl{
    class ReaderWriterLatch{
        using mutex_t = std::mutex;
        using cond_t = std::condition_variable;
    };
}
#endif //DTIMEDB_RWLATCH_H
