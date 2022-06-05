#ifndef _LOG_STREAM_H
#define _LOG_STREAM_H

#include <string>
#include <functional>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <string_view>
#include "log_buffer.h"

namespace Log
{ 
    //日志流类 前端
    class LogStream
    {
    public:
        LogStream &operator<<(const char* str)//c字符
        {
            if (str)
                streamBuf_.writeIn(str, strlen(str));
            else
                streamBuf_.writeIn("(null)", 6);
            return *this;
        }

        LogStream &operator<<(std::string_view str)//cpp17
        {
            streamBuf_.writeIn(str.data(), str.size());
            return *this;
        }

        LogStream &operator<<(std::string&& str)
        {
            streamBuf_.writeIn(str.c_str(), str.size());
            return *this;
        }
        LogStream &operator<<(short);
        LogStream &operator<<(int);
        LogStream &operator<<(long);
        LogStream &operator<<(long long);
        LogStream &operator<<(unsigned short);
        LogStream &operator<<(unsigned int);
        LogStream &operator<<(unsigned long);
        LogStream &operator<<(unsigned long long);
        LogStream &operator<<(float);
        LogStream &operator<<(double);

        LogStream &operator<<(LogStream &(*fun)(LogStream &))//流结束符
        {
            fun(*this);
            return *this;
        }
        template <typename T>
        size_t convert(char buf[], T value);//数字转换字符串，返回字符串长度
        friend LogStream& end(LogStream& stream);//流结束符
        void setCallBack(const std::function<void(char*, int)>& cb){updateCallBack_ = cb;}
    private:        
        std::function<void(char*, int)> updateCallBack_;
        static const int maxNumLength; // 数字转化为字符串的最大长度。
        static const short maxLineSize; // 整行记录的最大长度。
        LogBuffer streamBuf_;//日志缓冲
    };
}

#endif