#include "log_thread.h"
#include "logger.h"
#include "local_message.h"
#include <chrono>

namespace Log
{
    const char *overFlowMes = "LOG FATAL: buf over flow!\n";

    LogThread::LogThread()
        : logger_(new Logger(std::bind(&LogThread::apendMessage, this, std::placeholders::_1, std::placeholders::_2))),
          currentBuf_(bufSize_),
          preBuf_(bufNum_),
          currentBufIndex_(0)
    {
        // currentBuf_.reserve(bufSize_ + 10);//0603
        Logger::resizeBuf();
        assert(bufNum_ >= 2);
        for (int i = 0; i < bufNum_ - 1; i++)
            preBuf_.push(std::vector<char>(bufSize_));
        auto config = Base::LocalMessage::entity();
        auto printMes = config->configRead("print log message");
        printMes_ = printMes == "yes";
        auto logThread = std::thread(std::bind(&LogThread::LogLoop, this));//创建后端线程
        logThread.detach();//分离
    }
    LogThread::~LogThread(){}
    LogThread* LogThread::init()//单例
    {
        if (!entity)
        {
            entity = new LogThread;
        }
        return entity;
    }

    void LogThread::apendMessage(char* buf, unsigned int size)//log_stream的回调
    {
        bool isChangeBuf = false;
        std::unique_lock<std::mutex> guard(mut_);
        auto freeSize = getFreeSize();
        assert(freeSize >= 0);
        if (freeSize < (int)size)
        {
            isChangeBuf = true;
            changeBuf(); //更换新的缓存
        }
        ::memcpy(&currentBuf_[0] + currentBufIndex_, buf, size);//亲测有效
        currentBufIndex_ += size;
        if (isChangeBuf)
        {
            mut_.unlock();//unique_lock提供解锁
            waiter_.notify_one(); //唤醒工作线程
        }
    }
    void LogThread::changeBuf()
    {
        auto buf = preBuf_.try_pop();
        if (buf != std::nullopt)//preBuf_还有空闲内存
        {//exchange cpp14 右面替换左面，返回左面的初值 readyBuf_(...) <-currentBuf_ <=>preBuf_(...).pop()
            readyBuf_.emplace_back(std::exchange(currentBuf_, std::move(buf.value())), currentBufIndex_);
            currentBufIndex_ = 0;
        }
        else //如果没有空闲内存了，直接覆盖currentBuf_的一个最旧的数据
        {
            ::strcpy(currentBuf_.data(), overFlowMes); //在首元素处插入丢弃数据的信息。
            currentBufIndex_ = sizeof(overFlowMes);
            reloadMes_ = true;
        }
    }
    void LogThread::LogLoop()//后端写文件线程执行 
    {
        bool logging_ = true;
        auto beginTime  = std::chrono::system_clock::now();  // 记录本次loop开始的时间
        auto waitTime   = std::chrono::system_clock::now();  // 记录从条件变量唤醒的时间
        auto finishTime = std::chrono::system_clock::now(); // 记录本次loop结束的时间
        std::cout << "Log thread begin...." << std::endl;
        while (logging_)
        {
            beginTime = std::chrono::system_clock::now();
            {
                std::unique_lock<std::mutex> guard(mut_);
                while (readyBuf_.empty())
                    waiter_.wait(guard);
                waitTime = std::chrono::system_clock::now();
                assert(bufToWrite_.empty());
                bufToWrite_.swap(readyBuf_); //交换缓冲区 bufToWrite_(...) <=>readyBuf_(...)
            }
            assert(bufToWrite_.size() <= bufNum_);
            flushToFile(); // 落盘，写文件的缓存里
            logFile_.flush();//刷盘，真正写入文件
            finishTime = std::chrono::system_clock::now();
            loopTime_ = std::chrono::duration_cast<std::chrono::microseconds>
                        (finishTime - beginTime).count();
            auto waitTemp = std::chrono::duration_cast<std::chrono::microseconds>
                        (waitTime - beginTime).count();
            //更新比例
            loopRatio_ = (1 - static_cast<double>(waitTemp) / loopTime_) * 100;
            writeLogMes();
        }
    }
    void LogThread::writeLogMes()//上次日志调试信息写入下次日志中
    {
        static char buf[512] = {0};
        if(!printMes_) return;
        ::snprintf(buf, 256,
            "\r\nLog Message: Cost time: %5.3lfms, write bytes: %5.3lfKb, Log reload: %d, Work ratio: %5.3lf%%, Speeds: %5.3lfMb/s\r\n",
            static_cast<double>(loopTime_) / 1000,
            static_cast<double>(writeMes_) / 1000,
            reloadMes_.load(),
            loopRatio_,
            static_cast<double>(writeMes_) / loopTime_);
        reloadMes_ = false;
        apendMessage(buf, ::strlen(buf));
    }

    void LogThread::flushToFile()//bufToWrite_队列写入LogFile缓存
    {
        int size = bufToWrite_.size();
        writeMes_ = 0;
        for (int i = 0; i < size; i++)
        {
            auto temp = std::move(bufToWrite_.back());
            bufToWrite_.pop_back();
            logFile_.writeMessage(temp.first.data(), temp.second);
            writeMes_ += temp.second;
            preBuf_.push(std::move(temp.first));//写完了放回preBuf_
        }
    }
}
