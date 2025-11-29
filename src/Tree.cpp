#include "../include/Tree.h"
#include "../include/Utils.h"
#include <sstream>

Tree::Tree() {}

std::string Tree::getType() const 
{
    return "tree";
}

std::vector<unsigned char> Tree::serialize() const 
{
    std::stringstream data;
    data << entries.size();
    
    for (const auto& entry : entries) 
    {
        // "fileName blobHash"
        data << " " << entry.first << " " << entry.second;
    }
    
    std::string dataStr = data.str();
    return std::vector<unsigned char>(dataStr.begin(), dataStr.end());
}

void Tree::deserialize(const std::vector<unsigned char>& data) 
{
    entries.clear(); //Only contain the latest deserialized data
    std::string dataStr(data.begin(), data.end());
    std::istringstream stream(dataStr);
    
    size_t filecnt;
    stream >> filecnt;

    for (int i = 0; i < filecnt; ++i)
    {
        std::string fileName, blobHash;
        stream >> fileName >> blobHash;
        entries[fileName] = blobHash;
    }
}

void Tree::addFile(const std::string& fileName, const std::string& blobHash) 
{
    entries[fileName] = blobHash;
}
void Tree::deleteFile(const std::string& fileName) 
{
    entries.erase(fileName);
}
bool Tree::existFile(const std::string& fileName) const 
{
    return entries.find(fileName) != entries.end();
}

std::string Tree::getFileHash(const std::string& fileName) const 
{
    auto it = entries.find(fileName);
    if (it != entries.end()) 
    {
        return it->second;
    }
    throw std::runtime_error("File not found in Tree: " + fileName);
}
std::vector<std::string> Tree::getFileNames() const 
{
    std::vector<std::string> fileNames;
    for (const auto& entry : entries) 
    {
        fileNames.push_back(entry.first);
    }
    std::sort(fileNames.begin(), fileNames.end());
    return fileNames;
}
std::map<std::string, std::string> Tree::getAllEntries() const 
{
    return entries;
}

void Tree::clear() 
{
    entries.clear();
}
size_t Tree::getFileCount() const 
{
    return entries.size();
}
bool Tree::isEmpty() const 
{
    return entries.empty();
}