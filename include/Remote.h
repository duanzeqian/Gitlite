/*
    远程仓库信息存储
*/
#ifndef REMOTE_H
#define REMOTE_H

#include <string>

class Remote // each Remote object stores information of ONE remote repository
{
private:
    std::string name;
    std::string path;
    
public:
    Remote() = default;
    Remote(const std::string& repoName, const std::string& path);

    // getters
    std::string getName() const;
    std::string getPath() const; // get the path of repository
    std::string getBranchPath(const std::string& branchName) const; // get the path of branch
    bool repoExists() const;
};

#endif // REMOTE_H