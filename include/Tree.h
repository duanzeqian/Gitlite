/*
    Tree：将名称映射到对 Blob 和其他树（子目录）的引用的目录结构；
    在我们实现的弱化版GitLite中，将Tree合并到Commit中而不处理子目录,
    （因此每个存储库都会有一个纯文件“平面”目录）。 
*/
#ifndef TREE_H
#define TREE_H

#include "GitliteObject.h"
#include <vector>
#include <string>
#include <map>

class Tree : public GitliteObject
{
private:
    std::map<std::string, std::string> entries; // fileName -> blobHash

public:
    Tree();
    
    // pure virtual from GitliteObject
    std::string getType() const override;
    std::vector<unsigned char> serialize() const override;
    void deserialize(const std::vector<unsigned char>& data) override;
    
    // files
    void addFile(const std::string& fileName, const std::string& blobHash); // add this file
    void deleteFile(const std::string& fileName); // delete this file
    bool existFile(const std::string& fileName) const; // whether this file exists

    // getter
    std::string getFileHash(const std::string& fileName) const;
    std::vector<std::string> getFileNames() const;
    std::map<std::string, std::string> getAllEntries() const;

    // map
    void clear();
    size_t getFileCount() const;
    bool isEmpty() const;
};

#endif // TREE_H