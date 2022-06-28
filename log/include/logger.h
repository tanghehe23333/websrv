#ifndef _LOGGER_H
#define _LOGGER_H

#include <functional>
#include <cassert>
#include <iostream>
#include "log_stream.h"
#include "log_thread.h"

namespace Log
{
  extern  LogThread* entity; 
  LogThread* init();//全局单例
  enum LogLevel
  {
    INFO_,
    DEBUG_,
    FATAL_,
    HTTP_
  };

  class Logger
  {
  public:
    Logger(std::function<void(char*, int)>);
    constexpr char const* levelToStr(Log::LogLevel logLevel);
    LogStream& receive(const char* fileName, int line, const char* funName, Log::LogLevel logLevel);
    unsigned int findFileName(const char*);
    std::string dealFunName(const char* funName);
    static void resizeBuf()
    {
      fileName_.resize(maxNameSize_);
      funName_.resize(maxNameSize_);
    }
  private:
    void dealLogStr(const char*, std::string& buf, int maxSize = 15);
  private:
    inline static thread_local std::string fileName_;  //文件名 线程安全
    inline static thread_local std::string funName_;   //函数名 线程安全
    LogStream stream_;//日志流
    static const int maxNameSize_ = 25; //文件名等缓冲区的最大容量
  };

  LogStream& end(LogStream& stream);  //流结束标志

}

const bool enableHttpDebug = true;
// constexpr Log::LogLevel logLevel = Log::LogLevel::INFO_;

//暂时先设计成不支持动态变化日志等级的模式
#define LOG_INFO                        \
  Log::entity->logger_->receive(__FILE__, __LINE__, __func__, Log::INFO_)

#define LOG_DEBUG                        \
  Log::entity->logger_->receive(__FILE__, __LINE__, __func__, Log::DEBUG_)

#define LOG_FATAL                        \
  Log::entity->logger_->receive(__FILE__, __LINE__, __func__, Log::FATAL_)

#define LOG_HTTP       \
  if constexpr(enableHttpDebug) \
  Log::entity->logger_ ->receive(__FILE__, __LINE__, __func__, Log::HTTP_)

#define LOG_COUT std::cout


#endif
