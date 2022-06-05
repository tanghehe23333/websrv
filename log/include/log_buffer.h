#ifndef _LOG_BUFFER_H
#define _LOG_BUFFER_H

#include <map>
#include "string.h"

namespace Log
{
    constexpr int bufferSize    = 10 * 1024;//10kB
    constexpr int headLevel     = bufferSize - 3;//高水位线 \r \n
    constexpr int lowLevel      = bufferSize - 256;//低水位线
    //日志缓冲类
    class LogBuffer 
    {
    public:
        LogBuffer(){}
        bool writeIn(const char* line, unsigned int size);
        bool writeEnd();
        std::pair<char*, int> updateMes(); //返回当前缓冲索引
        void refresh(){writeIndex_ = 0;}
    private:
        inline static thread_local char data_[bufferSize] = {0};
        inline static thread_local int writeIndex_;
    };
}
#endif