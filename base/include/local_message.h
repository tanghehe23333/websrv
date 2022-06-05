#ifndef _LOCAL_MASSAGE_H
#define _LOCAL_MASSAGE_H

#include <string>
#include <filesystem>
#include <map>
#include <sys/syscall.h>
#include <unistd.h>
#include "file_read.h"

extern thread_local int thread_tid;//线程独立

inline int getTid()
{
    if (__builtin_expect(thread_tid == 0, 0))//只有第一次才成立
    {
        thread_tid = static_cast<int>(::syscall(SYS_gettid));
    }
    return thread_tid;
}

namespace Base
{
    inline const int MOD_LT = 0;
    inline const int MOD_ET = 1;
    //服务器配置类
    class LocalMessage : public FileRead
    {
    public:
        static LocalMessage* readConfig(const std::filesystem::path& path);//getinstance()
        ~LocalMessage();
        static LocalMessage* entity(){return entity_;}
        static int port() {return linstenPort_;}
        static std::string& htmlPath() {return htmlPath_;}
        static std::string& logPath() {return logPath_;}
        static int triggerMod() {return triggerMod_;}
        std::string configRead(const char* str);//供其他类查询配置的接口
        std::string configRead(const std::string& str);
    private:
        void prinfMes();
        LocalMessage(const std::filesystem::path& configFile);
    private:
        std::filesystem::path configFile_;
        std::map<std::string, std::string> confMap_;//储存配置信息
        static inline int linstenPort_ = -1;
        static inline std::string htmlPath_;
        static inline std::string logPath_;
        static inline int triggerMod_ = MOD_LT;
        static inline LocalMessage* entity_; //单例实体
    };

}

#endif
