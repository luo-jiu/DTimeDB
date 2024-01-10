//
// Created by illumwang on 24-1-1.
//
#include <chrono>
#include "type.h"

namespace ctl{
    class TimestampType:public Type{
    public:
        ~TimestampType() override=default;
        TimestampType(){
            auto current_time=std::chrono::system_clock::now();
            auto time_point=std::chrono::system_clock::to_time_t(current_time);
            auto ns=std::chrono::duration_cast<std::chrono::nanoseconds>(current_time.time_since_epoch())%1000000000;
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(current_time.time_since_epoch()) % 1000;
            std::tm *localTime=std::localtime(&time_point);
            year_=localTime->tm_year+1900;
            month_=localTime->tm_mon+1;
            day_ = localTime->tm_mday;
            hour_ = localTime->tm_hour;
            minute_ = localTime->tm_min;
            second_ = localTime->tm_sec;
            millisecond_=ms.count();
            nanosecond_ = ns.count();
        }
        //对于时间戳的操作函数
        TimestampType getTimestamp()const;
    private:
        int             year_;
        int             month_;
        int             day_;
        int             hour_;
        int             minute_;
        int             second_;
        int             millisecond_;
        int             nanosecond_;
    };
}
