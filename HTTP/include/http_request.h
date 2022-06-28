#ifndef _HTTP_REQUEST_H
#define _HTTP_REQUEST_H

#include <string>
#include <map>
#include <memory>
#include <algorithm>
#include <sys/types.h>

class TCPConnection;
namespace Http
{
    extern const char* RN;//"\r\n\r\n"
    enum HttpMethod//http方法
    {
        Invalid,
        Get,
        Post,
    };
    enum HttpVersion//版本
    {
        Unknown,
        Http10,
        Http11
    };
    enum MesState
    {
        CanDeal,    //本条消息已经读取成功
        MoreMes,    //本条消息还没有读完
        badMes      //无法理解的消息

    };
//解析一个http报文
    class HttpRequest
    {
    public:
        HttpRequest()
        :   request_(Invalid), 
            version_(Unknown),
            httpState_(empty_)
        {}
        std::pair<MesState, char*> readMessage(TCPConnection* conn);
        void display();
        HttpMethod request() const {return request_;}
        const std::string& query() const {return query_;}
        const std::string& entity() const {return entity_;}
        void clear();
    private:
        bool getRequestLine(char* start, char* end);
        bool setHttpMethod(const char* start, const char* end);
        bool setHttpVersion(char* start, char* end);
        char* getHeaderFields(char* start, char* end);
        char* findMessageHead(TCPConnection* conn);
        std::string displayMode();
        std::string displayVersion();
    private:
        enum HttpState
        {
            empty_,
            needHead_,
            needEntity_,
            finish_
        };
    private:
        std::map<std::string, std::string> headerMap_; //头部字段的消息
        std::string query_;  //用户所需的URL      
        std::string entity_; //实体部分
        HttpMethod request_;
        HttpVersion version_;
        HttpState httpState_;
        static const size_t maxRequestSize = 2048; //
    };
}
#endif