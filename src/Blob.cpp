#include "../include/Blob.h"
#include "../include/Utils.h"

Blob::Blob() {}
Blob::Blob(const std::vector<unsigned char>& data) : content(data) {}
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
    return content;
}
void Blob::deserialize(const std::vector<unsigned char>& data) 
{
    content = data;
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