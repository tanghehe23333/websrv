#include "tcp_buffer.h"
#include "assert.h"
#include "logger.h"
#include "tcp_buff_pool.h"
#include "local_message.h"

thread_local std::shared_ptr<BufferPool> Buffer::bufferPool_ = nullptr;

Buffer::Buffer()
    : readIndex1_(0), readIndex2_(0), sendIndex1_(0), sendIndex2_(0)
{
    assert(bufferPool_ != nullptr);
    ReadBufSize_ = bufferPool_->getBuf(readBuf_, initSize_);
    SendBufSize_ = bufferPool_->getBuf(sendBuf_, initSize_);
    assert(readBuf_ != nullptr);
    assert(sendBuf_ != nullptr);
    assert(sendBuf_ != readBuf_);
}

Buffer::~Buffer()
{
    bufferPool_->freeBuf(readBuf_, ReadBufSize_);
    bufferPool_->freeBuf(sendBuf_, SendBufSize_);
    LOG_INFO << "buffer 析构" << Log::end;
}

bool Buffer::saveReadable(int fd, int mod)
{
    char buf[readSize_] = {0};
    LOG_INFO << "read begin: fd = " << fd << Log::end;
    int n = 0;
    if(mod == Base::MOD_LT)//for LT mode
        n = ::recv(fd, buf, readSize_ - 1, 0);
    else  // for ET mode.
    {
        int size;
        while((size = ::recv(fd, buf + n, readSize_ - 1, 0)) > 0) 
            n += size;
        if (size == -1 && errno != EAGAIN)
            LOG_FATAL << "read error" << Log::end;    
    }
    
    if (n <= 0)
        return false;//出错或对方关闭连接
    
    if (readIndex1_)
        LOG_DEBUG << "readIndex1_: " << readIndex1_ << Log::end;
    if (readIndex2_ + n > ReadBufSize_)//如果当前剩余空间不够，就换一块
    {
        int resize = readIndex2_ - readIndex1_ + 1 + n;
        LOG_DEBUG << "read Buf need resize: " << resize << Log::end;
        char* temp;
        int preSize = ReadBufSize_;
        ReadBufSize_ = bufferPool_->getBuf(temp, resize);
        assert(temp != nullptr);
        assert(readIndex2_ - readIndex1_ <= ReadBufSize_);
        std::copy(readBuf_ + readIndex1_, readBuf_ + readIndex2_, temp);
        bufferPool_->freeBuf(readBuf_, preSize);
        readIndex2_ -= readIndex1_;
        readIndex1_ = 0;
        readBuf_ = temp;
    }
    std::copy(buf, buf + n, readBuf_ + readIndex2_);
    readIndex2_ += n;
    return true;
}

void Buffer::clear()
{
    readIndex1_ = 0;
    readIndex2_ = 0;
}

void Buffer::clear(char* index)
{    
    if (index == readBuf_ + readIndex2_) //全清
    {
        readIndex1_ = 0;
        readIndex2_ = 0;
    }
    else 
    {
        readIndex1_ = std::distance(readBuf_, index);//index之前的清除
        LOG_DEBUG << "part buffer clear " << Log::end;
    }
}

bool Buffer::sendSendable(int fd)
{
    if(sendIndex2_ == sendIndex1_) return true;
    int size = sendIndex2_ - sendIndex1_;
    auto n = ::send(fd, sendBuf_ + sendIndex1_, size, 0);
    if (n == size)
    {
        LOG_INFO << "Buffer: 发送完成!" << Log::end;
        sendIndex1_ = 0;
        sendIndex2_ = 0;
        return true;
    }
    else
    {
        LOG_DEBUG << "Buffer: 还得接着发 ~QAQ~" << Log::end;
        sendIndex1_ += n;
        return false;
    }
}

void Buffer::sendMessage(const std::string& mes) //缓存数据到sendbuf
{
    assert(sendIndex2_ == 0 || sendIndex1_);
    int n = mes.size();    
    if(n > maxSendSize_)
        return;    
    if (sendIndex2_ + n > SendBufSize_)//常规扩容
    {
        int resize = sendIndex2_ - sendIndex1_ + 1 + n;
        LOG_DEBUG << "send Buf need resize: " << resize << Log::end;
        char* temp;
        int preSize = SendBufSize_;
        SendBufSize_ = bufferPool_->getBuf(temp, resize);
        assert(temp != nullptr);
        assert(sendIndex2_ - sendIndex1_ <= SendBufSize_);
        std::copy(sendBuf_ + sendIndex1_, sendBuf_ + sendIndex2_, temp);
        bufferPool_->freeBuf(readBuf_, preSize);
        sendIndex2_ -= sendIndex1_;
        sendIndex1_ = 0;
        sendBuf_ = temp;
    }
        std::copy(mes.begin(), mes.end(), sendBuf_ + sendIndex2_);
        sendIndex2_ += n;
}
