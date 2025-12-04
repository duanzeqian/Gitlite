#include "../include/Remote.h"
#include "../include/Utils.h"

Remote::Remote(const std::string& repoName, const std::string& path) 
    : name(repoName), path(path)
{}

std::string Remote::getName() const { return name; }
std::string Remote::getPath() const { return path; }
std::string Remote::getBranchPath(const std::string& branchName) const { return Utils::join(path, "refs", "heads", branchName); }

bool Remote::repoExists() const
{
    if (path.empty()) return false;
    if (!Utils::exists(path) || !Utils::isDirectory(path)) return false;
    
    std::string objectsDir = Utils::join(path, "objects");
    std::string refsDir = Utils::join(path, "refs");
    
    return Utils::exists(objectsDir) && Utils::exists(refsDir);
}