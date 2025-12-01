#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include "GitliteObject.h"
#include "Blob.h"
#include "Tree.h"
#include "Commit.h"

class Repository 
{
private:
    std::string workTree; // working directory
    std::string gitDir;   // .gitlite
    
    std::string currentBranch; // current branch
    std::map<std::string, std::string> branches; // branchName -> commitHash
    
    Tree stagingArea; // staging area

    std::set<std::string> rmFiles; // files marked for removal (in Commitcmd and Rm)

    // save and load changes in certain operation
    void saveStagingArea() const;
    void loadStagingArea();
    void saveRmFiles() const;
    void loadRmFiles();

    Repository(const Repository&) = delete;
    Repository& operator=(const Repository&) = delete;

public:
    Repository();
    
    // initializer
    void initialize(const std::string& path = ".");
    bool isInitialized() const;
    
    // getters
    std::string getGitliteDir() const;
    std::string getWorkTree() const;
    std::string getCurrentBranch() const;
    std::vector<std::string> getAllBranches() const;
    std::string getBranchHead(const std::string& branchName) const;
    Tree getStagingArea() const;
    std::set<std::string> getRmFiles() const;
    
    // operations on branches
    void setCurrentBranch(const std::string& branchName);
    void createBranch(const std::string& branchName, const std::string& startPoint = "");
    void deleteBranch(const std::string& branchName);
    bool branchExists(const std::string& branchName) const;
    void setBranchHead(const std::string& branchName, const std::string& commitHash);
    std::string findLCA(const std::string& currentBranch, const std::string& givenBranch) const;
    
    // operations on head
    void setHead(const std::string& ref);
    std::string getHead() const;
    std::string resolveHead() const; // find the commit that Head points to
    
    // operations on staging area
    void stageFile(const std::string& fileName, const std::vector<unsigned char>& data);
    void stageFile(const std::string& fileName, const std::string& content);
    void unstageFile(const std::string& fileName);
    void clearStagingArea();
    bool isStaged(const std::string& fileName) const;
    
    // operations on GitliteObjects
    std::string storeObject(const GitliteObject& obj) const;
    std::unique_ptr<GitliteObject> readObject(const std::string& hash) const;
    std::unique_ptr<Blob> readBlob(const std::string& hash) const;
    std::unique_ptr<Tree> readTree(const std::string& hash) const;
    std::unique_ptr<Commit> readCommit(const std::string& hash) const;
    bool objectExists(const std::string& hash) const;
    
    // operations on Commit
    void createInitialCommit();
    std::string createCommit(const std::string& message, const std::vector<std::string>& fatherHashes = {});
    
    // getter on files
    std::vector<std::string> getTrackedFiles() const;
    std::vector<std::string> getUntrackedFiles() const;
    std::vector<std::string> getModifiedFiles() const;
    bool isTracked(const std::string& fileName) const;

    // operations on files marked for removal
    bool hasRmTag(const std::string& fileName) const;
    void addRmTag(const std::string& fileName);
    void deleteRmTag(const std::string& fileName);
    void clearAllRmTag();

    // used for debug
    void debugPrintTrackedFiles() const;

private:
    std::string createTree() const;
    std::string getObjectPath(const std::string& hash) const;
    std::string getBranchPath(const std::string& branchName) const;
    void writeHeadFile(const std::vector<std::string>& lines);
};

#endif // REPOSITORY_H
