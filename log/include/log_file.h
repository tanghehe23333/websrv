#ifndef _LOG_FILE_H
#define _LOG_FILE_H

#include <string>
#include <sys/types.h>
#include <assert.h>
#include <string.h>
#include <iostream>

namespace Log
{
    //日志文件类
    class LogFile
    {
    public:
        LogFile();
        ~LogFile();
        void writeMessage(const char* message, const size_t len);
        off_t writtenBytes_() {return writeBytes_;}
        void flush();
    private:
        void changeFile();
        void addFileNumber(char*);
        FILE* fp_;
        char buffer_[1 * 1024]; //文件流的缓冲区 -> fileName_ 64KB  1kB 0603
        off_t writeBytes_;//文件写入偏移
        const unsigned long maxSizePeerFile_;
        int fileCount_; //文件数量
        std::string fileName_;//文件路径 
    };
}
#endif