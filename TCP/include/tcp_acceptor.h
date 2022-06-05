#ifndef _TCP_ACCEPTOR_H
#define _TCP_ACCEPTOR_H

#include "tcp_addr.h"
#include <functional>
#include <atomic>
#include <thread>
//抽象基类
class Acceptor
{
public:
    using FunctionType = std::function<void(int, std::chrono::time_point<std::chrono::system_clock>)>;
    Acceptor(FunctionType);
    virtual  ~Acceptor();
    bool begin(int fd);
    bool begin() {return begin(createListenSocket());}
    const auto fd() const {return listenFd_;}
    void closeLoop() {looping_ = false;};    //直接将标志位置零
protected:
    virtual bool acceptInit() = 0;  //初始化操作
    virtual void acceptLoop() = 0;  //接收操作
    virtual int createListenSocket();
protected:
    int listenFd_;//监听 
    FunctionType submitCallBack_;
private:
    void Loop();//eventLoop
private:
    std::atomic<bool> looping_;
    std::thread acceptThread_;//监听线程
};

#endif