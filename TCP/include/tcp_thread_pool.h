#ifndef _TCP_THREAD_POOL_H
#define _TCP_THREAD_POOL_H

#include <vector>
#include <thread>
#include <functional>
#include <memory>
#include <mutex>
#include <assert.h>

#include "tcp_thread_pool_process.h"

class TCPConnection;

#if 0  //抽象基类，先不用
class PoolFunction
{
public:
    virtual void operator()() = 0;
    virtual void addConnection(const TCPConnection&);
    virtual void close();
};
#endif
//线程池类
class ThreadPool
{    
public:
    ~ThreadPool();
    static std::shared_ptr<ThreadPool> init(int threadNum);
    void pushConnect(const int fd);
    void begin();
private:
    ThreadPool(int threadNum)
        :current_(0),
        threadNum_(threadNum) 
    {}
    std::vector<std::shared_ptr<PoolProcess>> poolProcesses_;//池
    std::vector<std::thread> threadList;//池
    int current_;//轮询
private:
    inline static std::shared_ptr<ThreadPool> entity_ = nullptr;
    const int threadNum_;
};

#endif