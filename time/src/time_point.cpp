#include "time_point.h"

namespace Time
{
    std::string timePoint::toString()//second.microsecond
    {        
        auto temp = time_point_cast<milliseconds>(time_);        
        auto value = temp.time_since_epoch().count();        
        char buf[32] = {0};
        snprintf(buf, sizeof(buf), "%ld.%06ld", (value / 1'000'000) % 60, (value) % 1000'000);
        return buf;
    }

    std::string timePoint::toLogString()//hour:minute:second.microsecond
    {
        struct tm tm_time;        
        auto temp = time_point_cast<microseconds>(time_);        
        auto count = temp.time_since_epoch().count();
        auto value = count / 1'000'000 + 8 * 3600;
        gmtime_r(&value, &tm_time);
        char buf[64];
        snprintf(buf, sizeof(buf), "%02d:%02d:%02d.%06ld",
                 tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
                 (count) % 1'000'000);
        std::string str(buf);
        return str;
    }
    std::string timePoint::toLogFileName()
    {
        struct tm tm_time;
        auto temp = time_point_cast<seconds>(time_);        
        auto value = temp.time_since_epoch().count();
        gmtime_r(&value, &tm_time);
        char buf[64] = {0};
        // snprintf(buf, sizeof(buf), "%4d-%02d-%02d-%02d-%02d",
        //         tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
        //         tm_time.tm_hour,tm_time.tm_min);
        snprintf(buf, sizeof(buf), "%4d-%02d-%02d",
                tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday);
        return buf;
    }
}
