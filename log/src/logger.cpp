#include "logger.h"
#include "time_point.h"
#include "local_message.h"

#include <algorithm>

namespace Log
{
    LogThread* entity = nullptr;
    const int logMessageSize = 384; //每条日志的最大长度
    char logMessageBuffer[logMessageSize];
    LogThread* init()//名称空间的init()
    {
        return LogThread::init();//LogThread的init()
    }

    Logger::Logger(std::function<void(char*, int)> cb)
    {
        stream_.setCallBack(cb);
    }

    constexpr char const* Logger::levelToStr()
    {
        switch (logLevel)
        {
        case INFO_:
            return "Info: ";
        case DEBUG_:
            return "Debug: ";
        case FATAL_:
            return "Fatal: ";
        case HTTP_:
            return "HTTP: ";
        }
        return nullptr;
    }

    unsigned int Logger::findFileName(const char* FileName)
    {
        const char* slash = strrchr(FileName, '/');//找'/'的位置
        assert(slash);
        return static_cast<int>(slash - FileName) + 1;
    }

    void Logger::dealLogStr(const char* str, std::string& buf, int maxSize) //将字符串格式化成固定长度
    {
        maxSize = std::min(maxSize, maxNameSize_ - 1);
        int size = static_cast<int>(strlen(str));
        size = std::min(size, maxSize);
        ::memcpy(&buf[0], str, size);        
        if(maxSize > size)
        {
            ::memcpy(&buf[size], "                                                               ", maxSize - size);
        } 
        buf[maxSize] = '\0';
    }

    //格式化字符串
    LogStream& Logger::receive(const char* fileName, int line, const char* funName)
    {
        std::cout << " get input /n" << std::endl;//0603
        Time::timePoint time(Time::GetNowTime());
        dealLogStr(fileName + findFileName(fileName), fileName_);//fileName格式化
        dealLogStr(funName, funName_);//funName格式化
        stream_ << time.toLogString() << "\t" << getTid() << "\t" << &fileName_[0] << "\t" << line << "\t" << &funName_[0] << "\t" << levelToStr();

        std::cout << time.toLogString() << "\t" << getTid() << "\t" << &fileName_[0] << "\t" << line << "\t" << &funName_[0] << "\t" << levelToStr() << std::endl;//0603

        return stream_;
    }

}
