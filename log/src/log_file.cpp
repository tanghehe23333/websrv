#include "log_file.h"
#include "time_point.h"
#include "local_message.h"

namespace Log
{
    LogFile::LogFile()
        : writeBytes_(0),
          maxSizePeerFile_(100 * 1024 * 1024), // 100MB
          fileCount_(1)
    {
        fileName_ =  Base::LocalMessage::logPath();
        Time::timePoint time(Time::GetNowTime());
        std::string name(fileName_);
        name += time.toLogFileName();//GMT时间
        char buf[16]={0};
        addFileNumber(buf);
        name += buf;        
        fp_ = ::fopen(name.c_str(), "a");//“a”
        assert(fp_);  //输入的日志保存路径失败
        //::setbuffer(fp_, buffer_, sizeof(buffer_));//设置文件流的缓冲
    }

    LogFile::~LogFile()
    {
        flush();//0603
        ::fclose(fp_);
    }

    void LogFile::writeMessage(const char* message, size_t len)
    {
        // std::cout << "write to LogFile buffer \n" << std::endl;
        assert(len >= 0);
        size_t written = 0;
        while (written != len)
        {
            size_t remain = len - written;
            auto n = ::fwrite_unlocked(message + written, 1, remain, fp_);//无锁速度快，moduo
            if (n != remain)
            {                
                if (ferror(fp_))
                    break;                
            }
            written += n;
        }
        writeBytes_ += written;
        if (static_cast<unsigned long>(writeBytes_) > maxSizePeerFile_)
        {
            changeFile();
        }
    }

    void LogFile::flush()
    {
        ::fflush(fp_);
    }

    void LogFile::changeFile()     //当文件写入数量足够的时候，换文件
    {
        std::cout << "changeFIle \n" << std::endl;
        flush();//0603
        Time::timePoint time(Time::GetNowTime());
        auto name = fileName_;
        name += time.toLogFileName();
        char buf[16] = {0};
        addFileNumber(buf);
        name += buf;
        ::fclose(fp_);
        fp_ = ::fopen(name.c_str(), "a");//"a"
        assert(fp_);
        // ::setbuffer(fp_, buffer_, sizeof(buffer_)); //0603
        writeBytes_ = 0;
    }

    void LogFile::addFileNumber(char* buf)
    {
        int temp = fileCount_++;
        *buf++ = '_';
        while (temp)
        {
            *buf++ = temp % 10 + '0';
            temp /= 10;
        }
        *buf++ = '.';
        *buf++ = 'L';
        *buf++ = 'o';
        *buf++ = 'g';
    }

}
