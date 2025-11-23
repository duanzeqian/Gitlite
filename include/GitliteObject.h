/*
    Gitlite对象基类
*/
#ifndef GITLITE_OBJECT_H
#define GITLITE_OBJECT_H

#include <vector>
#include <string>
#include <memory>

class Utils;

class GitliteObject
{
protected:
    std::string hash;//hash value of object

public:
    virtual ~GitliteObject() = default;
    
    virtual std::string getType() const = 0;//pure virtual, know the type
    
    virtual std::vector<unsigned char> serialize() const = 0;//pure virtual, serialize an object
    
    virtual void deserialize(const std::vector<unsigned char>& data) = 0;//pure virtual, deserialize an object
    
    virtual std::string computeHash();//compute the hash value of object
    
    std::string getHash() const;
    
    void setHash(const std::string& h);
    
    virtual size_t getSize() const;
};

#endif // GITLITE_OBJECT_H