#ifndef _BASE_FILE_H
#define _BASE_FILE_H

#include <string>
#include <fstream>
#include <filesystem>

namespace Base
{
    class FileRead
    {
    public:
        FileRead(const std::string& path);
        FileRead(const std::filesystem::path& path)
            :FileRead(path.string())
        {}
        void read(std::string& buf);
        bool readLine(std::string& mes)//读文件的一行，以'\n'为界，读完文件返回false
        {
            char buf[1024] = {0};
            file_.getline(buf, 1024, '\n');
            mes = buf;
            return !file_.eof();
        }
        ~FileRead();
        bool  isOpen()const {return isOpen_;}
        uint64_t size() const {return size_;}
        const std::filesystem::path& path() const {return  path_;}
    protected:
        void closeFile();
    private:
        uint64_t size_;
        bool isOpen_;
        std::ifstream file_;
        std::filesystem::path path_;//c++17,文件路径
    };
}

#endif