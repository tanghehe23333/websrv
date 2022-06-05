#ifndef _MYSQL_CONNECT_H
#define _MYSQL_CONNECT_H

#include <mysql/mysql.h>
#include <string>
#include <vector>
#include <chrono>
#include <memory>
#include "assert.h"

using namespace std::chrono;

namespace MySql_
{
    inline const char*  mySQL_IP        = "127.0.0.1";
    inline const char*  mySQL_User      = "root";
    inline const char*  mySQL_Password  = "2569158";
    inline const char*  mySQL_Database  = "webserver";
    inline const int    mySQL_Port      = 3306;
    //mysql的表项
    struct UserInfo
    {        
        UserInfo(const std::vector<std::string>& mes)
        :   id_(::atoi(mes[0].c_str())),
            name_(mes[1]),
            password_(mes[2]),
            signTime_(mes[3]),
            signNum_(::atoi(mes[4].c_str()))
        {}
        UserInfo() = default;
        void set(const std::vector<std::string>& mes)
        {
            assert(mes.size() == 5);

            id_         = ::atoi(mes[0].c_str());
            name_       = mes[1];
            password_   = mes[2];
            signTime_   = mes[3];
            signNum_    = ::atoi(mes[4].c_str());
        }
        unsigned int id_ = 0;       //用户ID      
        std::string name_;          //用户名
        std::string password_;      //用户密码
        std::string signTime_;      //注册时间
        unsigned int signNum_ = 0;  //登录次数
    };

    //mysql连接类,封装一个mysql连接
    class MySqlConn
    {
    public:
        //返回数据库数据的类型。
        using QueryResultType = std::vector<std::vector<std::string>>;
        MySqlConn();
        ~MySqlConn();
        //搜索数据库
        QueryResultType sendCommend(const std::string& mes);//查
        bool sendComWithoutAns(const std::string& mes);//增
    private:
        MYSQL conns_;//数据库连接
    };
    
    //处理业务的类
    class MySqlMess : public MySqlConn
    {
    public:
        //根据名字搜索
        std::shared_ptr<UserInfo> searchByName(const std::string& name);
        //按ID搜索 未实现
        UserInfo searchById(int);
        //插入新数据        
        bool insert(const std::string& name, const std::string& password, const time_point<system_clock>& time);
        //更新密码 未实现
        bool updatePassword();
        //按名字删除数据
        bool deleteByName(const std::string& name);
    };
}

#endif