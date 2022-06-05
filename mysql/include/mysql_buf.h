#ifndef _MYSQL_BUF_H
#define _MYSQL_BUF_H
#include <string>
#include <mutex>
#include <memory>
#include "LRU_cache.h"
#include "mysql_connect.h"
namespace MySql_
{
    inline int bufSize = 0;
    
    class MySqlBuf
    {
    public:
        MySqlBuf();
        std::shared_ptr<const UserInfo> searchPlayer(const std::string& name);
        bool newPlayer(const std::string& name, const std::string& password);
    private:
        std::mutex mut_;
        Base::LRU_Cache<std::string, std::shared_ptr<const UserInfo>> cache_;
        inline static thread_local MySqlMess* conn_ = nullptr;//线程变量
    };
}
#endif