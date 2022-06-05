#include "local_message.h"
#include "logger.h"

thread_local int thread_tid = 0;

namespace Base
{
    LocalMessage *LocalMessage::readConfig(const std::filesystem::path& path)
    {
        if (!entity_)
        {
            entity_ = new LocalMessage(path);
        }
        return entity_;
    }

    LocalMessage::LocalMessage(const std::filesystem::path& configFile)
        : FileRead(configFile), configFile_(configFile)
    {
        std::filesystem::directory_entry direEntry(configFile_);
        assert(direEntry.is_regular_file());
        std::string mes;
        while (readLine(mes))//解析配置文件的每一行
        {//配置格式->   冒号 + 空格
            if (mes == "")
                break;
            auto iter = std::find(mes.begin(), mes.end(), ':');
            confMap_[std::string(mes.begin(), iter)] = std::string(iter + 2, mes.end());
        }
        auto iter = confMap_.find("port");//监听端口配置
        if (iter == confMap_.end())
        {
            std::cout << "fatal error: linstening port not be found" << std::endl;
            std::terminate();
        }
        int port = ::atoi(iter->second.c_str());
        if (port <= 0 || port > 65535)
        {
            std::cout << "fatal error: invalid port: " << port << std::endl;
            std::terminate();
        }
        linstenPort_ = port;
        iter = confMap_.find("log file direction");//日志存放路径配置
        if (iter == confMap_.end())
        {
            std::cout << "fatal error: cannot find the log file path" << std::endl;
            std::terminate();
        }
        else
        {
            int index = iter->second.find_first_not_of(" ");
            logPath_ = iter->second.substr(index, iter->second.size());
        }
        iter = confMap_.find("html file direction");//html文件位置配置
        if (iter == confMap_.end())
        {
            std::cout << "fatal error: cannot find the html file path" << std::endl;
            std::terminate();
        }
        else
        {
            int index = iter->second.find_first_not_of(" ");
            htmlPath_ = iter->second.substr(index, iter->second.size());
        }
        iter = confMap_.find("trigger_mod");//触发模式配置
        if (iter != confMap_.end())
        {
            const auto &flag = iter->second;
            auto temp = flag.find("ET");
            if (temp < flag.size())
                triggerMod_ = MOD_ET;
        }
        iter = confMap_.find("print message");//是否输出配置信息？
        if (iter != confMap_.end())
        {
            const auto& flag = iter->second;
            auto temp = flag.find("yes");
            if (temp < flag.size())
                prinfMes();
        }
    }

    std::string LocalMessage::configRead(const char* str)
    {
        auto iter = confMap_.find(str);
        if(iter == confMap_.end())
            return std::string();
        return iter->second;
    }
    std::string LocalMessage::configRead(const std::string& str)
    {
        auto iter = confMap_.find(str);
        if(iter == confMap_.end())
            return std::string();
        return iter->second;
    }

    void LocalMessage::prinfMes()//配置打印到终端
    {
        for (auto& [key, value] : confMap_)
        {
            std::cout << key << ":\t" << value << std::endl;
        }
    }

    LocalMessage::~LocalMessage()
    {
    }
}