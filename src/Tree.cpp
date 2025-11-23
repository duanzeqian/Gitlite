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
    std::stringstream data("");
    
    for (const auto& entry : entries) 
    {
        //"100644 filename\0hashBin"
        data << "100644 " << entry.first << "\0";
        
        //hashHex -> hashBin
        std::string hashHex = entry.second;
        if (hashHex.length() != 40)
        {
            throw std::runtime_error("Wrong length of SHA1 hash: " + hashHex);
        }

        for (size_t i = 0; i < 40; i += 2) 
        {
            std::string byteStr = hashHex.substr(i, 2);
            char byte = static_cast<char>(std::stoi(byteStr, nullptr, 16));//base = 16 (Hex)
            data << byte;
        }
    }
    
    std::string dataStr = data.str();
    return std::vector<unsigned char>(dataStr.begin(), dataStr.end());
}

void Tree::deserialize(const std::vector<unsigned char>& data) 
{
    entries.clear(); //Only contain the latest deserialized data
    std::string dataStr(data.begin(), data.end());
    size_t pos = 0;
    
    while (pos < dataStr.size()) 
    {
        //"100644 filename\0hashBin"
        //mode = 100644
        size_t spacePos = dataStr.find(' ', pos);//seperate mode and filename
        if (spacePos == std::string::npos)
        {
            throw std::runtime_error("Can't find space when deserializing Tree");
        }
        std::string mode = dataStr.substr(pos, spacePos - pos);
        if (mode != "100644")
        {
            throw std::runtime_error("Invalid mode when deserializing Tree: " + mode);
        }
        
        //filename
        size_t nullPos = dataStr.find('\0', spacePos + 1);//seperate filename and hashBin
        if (nullPos == std::string::npos)
        {
            throw std::runtime_error("Can't find null when deserializing Tree");
        }
        
        std::string filename = dataStr.substr(spacePos + 1, nullPos - spacePos - 1);
        
        //hashBin (20 bytes)
        if (nullPos + 20 >= dataStr.size())
        {
            throw std::runtime_error("Insufficient length for hashBin when deserializing Tree:");
        }
        
        std::string hashBin = dataStr.substr(nullPos + 1, 20);
        std::stringstream hashHex;
        for (unsigned char c : hashBin) 
        {
            hashHex << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
        }
        std::string blobHash = hashHex.str();
        
        entries[filename] = blobHash;
        pos = nullPos + 21;//the next entry
    }
}

void Tree::addFile(const std::string& filename, const std::string& blobHash) 
{
    entries[filename] = blobHash;
}
void Tree::deleteFile(const std::string& filename) 
{
    entries.erase(filename);
}
bool Tree::existFile(const std::string& filename) const 
{
    return entries.find(filename) != entries.end();
}

std::string Tree::getFileHash(const std::string& filename) const 
{
    auto it = entries.find(filename);
    if (it != entries.end()) 
    {
        return it->second;
    }
    throw GitliteException("File not found in Tree: " + filename);
}
std::vector<std::string> Tree::getFilenames() const 
{
    std::vector<std::string> filenames;
    for (const auto& entry : entries) 
    {
        filenames.push_back(entry.first);
    }
    std::sort(filenames.begin(), filenames.end());
    return filenames;
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