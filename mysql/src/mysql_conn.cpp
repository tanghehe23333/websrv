#include "mysql_connect.h"
#include "logger.h"


namespace MySql_
{
/*----------------------------------------------MySqlConn----------------------------------------------*/
    MySqlConn::MySqlConn()
    {
        auto temp = mysql_init(&conns_);//创建mysql连接
        assert(temp != nullptr);
        auto ret = mysql_real_connect(
            &conns_,
            mySQL_IP,
            mySQL_User,
            mySQL_Password,
            mySQL_Database,
            mySQL_Port,
            nullptr,
            0);
        assert(ret == &conns_);
        mysql_query(&conns_, "set names gbk");//设置GBK编码
    }

    MySqlConn::~MySqlConn()
    {
        mysql_close(&conns_);
    }

    MySqlConn::QueryResultType MySqlConn::sendCommend(const std::string& mes)
    {
        auto ret = mysql_query(&conns_, mes.c_str());
        QueryResultType ans;
        if (ret)
            return ans; //如果错误则直接返回
        auto result = mysql_store_result(&conns_);
        int column = mysql_num_fields(result);//列数
        int col = mysql_num_rows(result);//行数
        for (int i = 0; i < col; i++)
        {
            MYSQL_ROW row = mysql_fetch_row(result);//结果集中取一行
            std::vector<std::string> temp;
            temp.reserve(column);//容量设置为列数
            for (int j = 0; j < column; j++)
            {
               temp.push_back(row[j]);
            }
            ans.push_back(std::move(temp));
        }
        return ans;
    }

    bool MySqlConn::sendComWithoutAns(const std::string& mes)
    { 
        auto res = mysql_query(&conns_, mes.c_str());
        if(res)
            return false;
        else
            return true;
    }
/*----------------------------------------------MySqlMess----------------------------------------------*/
    std::shared_ptr<UserInfo> MySqlMess::searchByName(const std::string& name)
    {
        const static std::string commendStr("select * from web where name = \'");
        std::string commend = commendStr + name + "\'";
        QueryResultType ans = sendCommend(commend);
        auto mes = std::make_shared<UserInfo>();
        if(ans.empty())
            return mes;
        if(ans.size() > 1) LOG_FATAL << "More than one message !" << Log::end;
        mes->set(ans[0]);
        return mes;
    }

    bool MySqlMess::insert(const std::string& name, const std::string& password, const time_point<system_clock>& time)
    {
        const static std::string commendStr("insert into web(name, password, lastSign,signNums) values(");
        static struct tm tm_time;
        auto tempTime = time_point_cast<seconds>(time);//
        long count = tempTime.time_since_epoch().count();
        gmtime_r(&count, &tm_time);
        char buf[64] = {0};
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
                 tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                 (tm_time.tm_hour + 8) % 24, tm_time.tm_min, tm_time.tm_sec);
        auto temp = commendStr + "\'" + name + "\', \'" + password + "\', \'" + buf + "\', 1)";
        return sendComWithoutAns(temp);        
    }

    bool MySqlMess::deleteByName(const std::string &name)
    {
        const static std::string commendStr
        ("delete from web where name = \'");
        auto temp = commendStr + name + "\'";
        return sendComWithoutAns(temp);
    }
}