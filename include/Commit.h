#ifndef COMMIT_H
#define COMMIT_H

#include "GitliteObject.h"
#include "GitliteException.h"
#include <vector>
#include <string>
#include <ctime>

class Commit : public GitliteObject
{
private:
    std::string treeHash;
    std::vector<std::string> fatherHashes; // May exist multiple father commits
    std::string message;
    std::time_t timestamp;

public:
    Commit();
    Commit(const std::string& treeHash, 
           const std::vector<std::string>& fatherHashes,
           const std::string& message,
           std::time_t timestamp = std::time(nullptr));
    
    //pure virtual from GitliteObject
    std::string getType() const override;
    std::vector<unsigned char> serialize() const override;
    void deserialize(const std::vector<unsigned char>& data) override;
    
    //getter
    std::string getTreeHash() const;
    std::vector<std::string> getFatherHashes() const;
    std::string getMessage() const;
    std::time_t getTimestamp() const;
    
    //setter
    void setTreeHash(const std::string& hash);
    void addFatherHash(const std::string& fatherHash);
    void setFatherHashes(const std::vector<std::string>& fathers);
    void setMessage(const std::string& msg);
    void setTimestamp(std::time_t time);
    
    //commit methods
    bool isMergeCommit() const;
    bool isInitialCommit() const;
    std::string getShortMessage() const;
    std::string getTime() const;
    std::string getMergeFather() const;
};

#endif // COMMIT_H