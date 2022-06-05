#include "mysql_buf.h"
#include "time_point.h"

#include <cassert>

namespace MySql_
{
    MySqlBuf::MySqlBuf()
        : cache_(20)
    {
        if (conn_ == nullptr)
            conn_ = new MySqlMess;
    }

    bool MySqlBuf::newPlayer(const std::string& name, const std::string& password)
    {
        auto time = Time::GetNowTime();
        return conn_->insert(name, password, time);
    }

    std::shared_ptr<const UserInfo> MySqlBuf::searchPlayer(const std::string& name)
    {
        assert(conn_ != nullptr);
        std::optional<std::shared_ptr<const UserInfo>> res;//cpp17
        {
            std::lock_guard guard(mut_);
            res = cache_.get(name); // 先查缓存
        }
        if (res != std::nullopt)
        {
            return res.value();
        }
        auto result = conn_->searchByName(name);//缓存没有，查数据库
        {
            std::lock_guard guard(mut_);
            cache_.put(name, result);
        }
        return result;
    }
}