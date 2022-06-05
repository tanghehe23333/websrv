#ifndef _TCP_BUFFER_H
#define _TCP_BUFFER_H

#include <vector>
#include <unistd.h>
#include <string>
#include <mutex>
#include <queue>
#include <memory>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>

class BufferPool;
class TCPConnection;

class Buffer//TCP Buffer
{
public:
    Buffer();
    ~Buffer();
    bool saveReadable(int fd, int mod);
    void clear(char* index);  //
    void clear();             //
    void sendMessage(const std::string& mes);
    bool sendSendable(int fd);
    bool sendSize(int n);//未实现
    bool empty(){return readIndex1_ == readIndex2_;}
    static void setPool(std::shared_ptr<BufferPool>& pool) {bufferPool_ = pool;}
public:
    friend class TCPConnection;
private:
    char* readBuf_;//读缓存
    char* sendBuf_;//写缓存
    int ReadBufSize_;
    int SendBufSize_;
    int readIndex1_;//实际读的位置
    int readIndex2_;//已接受，但还没读 
    int sendIndex1_;//实际发完的位置
    int sendIndex2_;//还没发的位置
    static const int readSize_ = 16 * 1000;
    static const int initSize_ = 2048;
    static thread_local  std::shared_ptr<BufferPool>  bufferPool_;//每个线程都有一个内存池
    static const int maxSendSize_ = 16384; //单次发送的最大限制。
};


#endif