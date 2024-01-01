//
// Created by illumwang on 23-12-26.
//


#pragma once
#include <mutex>
#include <condition_variable>

namespace ctl{
    class ReaderWriterLatch{
        using mutex_t = std::mutex;
        using cond_t = std::condition_variable;
        static const uint32_t MAX_READERS=42;
    public:
        ReaderWriterLatch()=default;
        ~ReaderWriterLatch(){std::lock_guard<mutex_t> guard(mutex_t);}
       /**
        * 写锁
        */
        void WLock(){
            std::unique_lock<mutex_t> latch(mutex_);
            /**
             * writer_end写锁持有情况，读者在写者持有锁的情况下等待
             */
            while (writer_end_){
                reader_.wait(latch);
            }
            writer_end_= true;
           while (read_count_>0){
               writer_.wait(latch);
           }
        }
        /**
         * 释放写锁
         */
        void WUnlock(){
            std::lock_guard<mutex_t> guard(mutex_);
            writer_end_= false;
            reader_.notify_all();
        }
        /**
         * 获取读锁
         */
         void RLock(){
             std::unique_lock<mutex_t> latch(mutex_);
            while (writer_end_||read_count_==MAX_READERS){
                reader_.wait(latch);
            }
             read_count_++;
         }
         /**
          * 释放读锁
          */
          void RUnlock(){
              std::lock_guard<mutex_t> guard(mutex_);
              read_count_--;
              /**
               * 如果有写者持有写锁，当前读者减少到0是环形一个等待的写者线程
               */
             if (writer_end_){
                 if (read_count_==0){
                     writer_.notify_one();
                 }
             } else{
                 if (read_count_==MAX_READERS-1){
                     reader_.notify_one();
                 }
             }
          }
    private:
        mutex_t                     mutex_;
        cond_t                        writer_;
        cond_t                        reader_;
        uint32_t                      read_count_{0};
        bool                            writer_end_{false};
    };
}
