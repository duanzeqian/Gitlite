#include "../include/Blob.h"
#include "../include/Utils.h"

Blob::Blob() {}
Blob::Blob(const std::vector<unsigned char>& data) : 
    content(data)
{}
Blob::Blob(const std::string& contentStr)
{
    content = std::vector<unsigned char>(contentStr.begin(), contentStr.end());
}

std::string Blob::getType() const 
{
    return "blob";
}
std::vector<unsigned char> Blob::serialize() const 
{
    // 文件名 字节数 ASCII码
    // a.txt 5 72 101 108 108 111 (a.txt "Hello")
    std::stringstream data;
    data << content.size();

    for (auto byte : content)
    {
        data << " " << static_cast<int>(byte);
    }
    std::string dataStr = data.str();
    return std::vector<unsigned char>(dataStr.begin(), dataStr.end());
}
void Blob::deserialize(const std::vector<unsigned char>& data) 
{
    std::string dataStr(data.begin(), data.end());
    std::istringstream stream(dataStr);

    content.clear();
    size_t byteCnt;
    stream >> byteCnt;

    for (int i = 0; i < byteCnt; ++i)
    {
        int byte;
        stream >> byte;
        content.push_back(static_cast<unsigned char>(byte));
    }
}

std::vector<unsigned char> Blob::getContent() const 
{
    return content;
}
std::string Blob::getContentAsString() const 
{
    return std::string(content.begin(), content.end());
}

void Blob::setContent(const std::vector<unsigned char>& data) 
{
    content = data;
}
void Blob::setContent(const std::string& contentStr) 
{
    content = std::vector<unsigned char>(contentStr.begin(), contentStr.end());
}

size_t Blob::getSize() const 
{
    return content.size();
}