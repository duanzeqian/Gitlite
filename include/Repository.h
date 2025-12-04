#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <memory>
#include <set>
#include "GitliteObject.h"
#include "Blob.h"
#include "Tree.h"
#include "Commit.h"
#include "Remote.h"

class Repository 
{
private:
    std::string workTree; // working directory
    std::string gitDir;   // .gitlite
    
    std::map<std::string, std::string> branches; // branchName -> commitHash
    
    Tree stagingArea; // staging area

    std::set<std::string> rmFiles; // files marked for removal (in Commitcmd and Rm)

    std::map<std::string, Remote> remotes; // remote repositories

    // save and load changes in certain operation
    void saveStagingArea() const;
    void loadStagingArea();
    void saveRmFiles() const;
    void loadRmFiles();
    void loadRemotes();
    void saveRemote(const std::string& name, const std::string& path);
    void removeRemote(const std::string& name);

    Repository(const Repository&) = delete;
    Repository& operator=(const Repository&) = delete;

public:
    Repository();
    Repository(const std::string& repoPath);
    
    // initializer
    void initialize();
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
    std::string createCommitInMerge(const std::string& message, const std::vector<std::string>& fatherHashes, const Tree& mergedTree);
    
    // getter on files and contents
    std::vector<std::string> getTrackedFiles() const;
    std::vector<std::string> getUntrackedFiles() const;
    std::vector<std::string> getModifiedFiles() const;
    std::string getWorkTreeFileContent(std::string& fileName);
    std::string getStagedFileContent(const std::string& fileName);
    std::string getCommitFileContent(const std::string& fileName, const std::string& commitHash);
    bool isInWorkTree(const std::string& fileName) const;
    bool isTracked(const std::string& fileName) const;

    // operations on files marked for removal
    bool hasRmTag(const std::string& fileName) const;
    void addRmTag(const std::string& fileName);
    void deleteRmTag(const std::string& fileName);
    void clearAllRmTag();

    // basic operations on remote repositories
    void addRemoteRepo(const std::string& repoName, const std::string& path);
    void deleteRemoteRepo(const std::string& repoName);
    bool remoteRepoExists(const std::string& repoName) const;
    Remote getRemoteRepo(const std::string& repoName) const;
    std::vector<std::string> getAllRemoteRepos() const;
    bool isAncestor(const std::string& ancestor, const std::string& descendant) const; // check if ancestor is the ancestor of descendant

    // connection between current and remote
    void copyObject(const Repository& srcRepo, const std::string& objectHash, const std::string& destRepoPath) const;
    void copyCommitHistory(const Repository& srcRepo, const std::string& startCommit, const std::string& destRepoPath) const;
    void copyTree(const Repository& srcRepo, const std::string& treeHash, const std::string& destRepoDir, std::unordered_set<std::string>& copied) const;

    // encode name with '/'
    std::string encodeBranchName(const std::string& branchName) const;

    // used for debug
    void debugPrintTrackedFiles() const;

private:
    std::string createTree() const;
    std::string createTreeInMerge(const Tree& mergedTree) const;
    std::string getObjectPath(const std::string& hash) const;
    std::string getBranchPath(const std::string& branchName) const;
    void writeHeadFile(const std::vector<std::string>& lines);
};

#endif // REPOSITORY_H
