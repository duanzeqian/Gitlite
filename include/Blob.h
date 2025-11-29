/*
    BLOB = Binary Large OBject 二进制大对象
    已保存的文件内容。由于 Git 保存了多个版本的文件，因此单个文件可能对应多个 Blob：
    每个 Blob 都在不同的提交中进行跟踪；每个Blob对象对应一个文件快照。 
*/
#ifndef BLOB_H
#define BLOB_H

#include "GitliteObject.h"
#include <vector>
#include <string>

class Blob : public GitliteObject
{
private:
    std::vector<unsigned char> content; // file content

public:
    Blob();
    Blob(const std::vector<unsigned char>& data); // construct from vector<unsigned char>
    Blob(const std::string& contentStr); // construct from string
    
    // pure virtual from GitliteObject
    std::string getType() const override;
    std::vector<unsigned char> serialize() const override;
    void deserialize(const std::vector<unsigned char>& data) override;
    
    // Blob
    // Getter
    std::vector<unsigned char> getContent() const;
    std::string getContentAsString() const;
    // Setter
    void setContent(const std::vector<unsigned char>& data);
    void setContent(const std::string& contentStr);
    
    size_t getSize() const override;
};

#endif // BLOB_H