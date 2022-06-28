#ifndef _HTTP_DEAL_H
#define _HTTP_DEAL_H

#include <memory>
#include <map>
#include <assert.h>

class TCPConnection;
class HtmlFile;

namespace Http
{
    class HttpRequest;
    class WebResources;
//业务逻辑
    class HttpDeal
    {
    public:
        HttpDeal(HttpRequest& quest, TCPConnection& conn)
        :quest_(quest),conn_(conn)
        {}
        bool dealQuest();
        static void setResource(WebResources* resource);
    private:
        void sendBadMessage();
        void readEntity(); //
        void sendFile(const std::string&);  //
        bool addNewPlayer();  //处理新用户注册的信息。
    private:
        std::map<std::string, std::string> entityMap_;
        HttpRequest&   quest_;//http报文     
        TCPConnection& conn_;//tcp连接
        static WebResources* resource_;  //网页资源，多线程读
    };
}

#endif