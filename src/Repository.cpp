#include "../include/Repository.h"
#include "../include/Utils.h"
#include <fstream>
#include <sstream>
#include <algorithm>

void Repository::saveStagingArea() const
{
    std::string indexPath = Utils::join(gitDir, "index");
    auto data = stagingArea.serialize();
    Utils::writeContents(indexPath, data);
}
void Repository::loadStagingArea()
{
    std::string indexPath = Utils::join(gitDir, "index");
    if (Utils::exists(indexPath))
    {
        auto data = Utils::readContents(indexPath);
        stagingArea.deserialize(data);
    }
}
void Repository::saveRmFiles() const
{
    std::string rmFilePath = Utils::join(gitDir, "REMOVE");
    std::string content;
    for (const auto& file : rmFiles)
    {
        content += file + "\n";
    }
    Utils::writeContents(rmFilePath, content);
}
void Repository::loadRmFiles()
{
    std::string rmFilePath = Utils::join(gitDir, "REMOVE");
    if (Utils::exists(rmFilePath))
    {
        auto data = Utils::readContents(rmFilePath);
        std::string content(data.begin(), data.end());
        
        rmFiles.clear();
        std::istringstream iss(content);
        std::string line;
        
        while (std::getline(iss, line))
        {
            if (!line.empty())
            {
                rmFiles.insert(line);
            }
        }
    }
}

Repository::Repository() 
    : workTree("."), gitDir(".gitlite"), currentBranch("master")
{
    loadStagingArea();
    loadRmFiles();
}

void Repository::initialize(const std::string& path)
{
    workTree = path;
    gitDir = Utils::join(path, ".gitlite");
    
    Utils::createDirectories(Utils::join(gitDir, "objects"));
    Utils::createDirectories(Utils::join(gitDir, "refs", "heads")); // initial directory structure
    
    createBranch("master"); // initial branch (master)
    setCurrentBranch("master");
    
    setHead("ref: refs/heads/master"); // initial head (towards master)

    createInitialCommit(); // initial commit
    loadStagingArea();
    loadRmFiles();
}

bool Repository::isInitialized() const
{
    return Utils::exists(gitDir) && Utils::isDirectory(gitDir);
}

// getters
std::string Repository::getGitliteDir() const { return gitDir; }
std::string Repository::getWorkTree() const { return workTree; }
std::string Repository::getCurrentBranch() const { return currentBranch; }
std::vector<std::string> Repository::getAllBranches() const
{
    std::vector<std::string> result;
    for (const auto& branch : branches)
    {
        result.push_back(branch.first);
    }
    return result;
}
std::string Repository::getBranchHead(const std::string& branchName) const
{
    std::string branchPath = getBranchPath(branchName);
    std::string content = Utils::readContentsAsString(branchPath);
    content.erase(std::remove(content.begin(), content.end(), '\n'), content.end()); // remove all '\n's in content
    return content;
}
Tree Repository::getStagingArea() const { return stagingArea; }
std::set<std::string> Repository::getRmFiles() const { return rmFiles; }

// operations on branches
void Repository::setCurrentBranch(const std::string& branchName)
{
    currentBranch = branchName;
    setHead("ref: refs/heads/" + branchName); // point Head to current branch
}

void Repository::createBranch(const std::string& branchName, const std::string& startPoint)
{
    if (branchExists(branchName))
    {
        throw GitliteException("A branch with that name already exists.");
    }

    std::string startCommit;
    if (startPoint == "") startCommit = resolveHead();
    else startCommit = startPoint;
    branches[branchName] = startCommit;
    
    std::string branchPath = getBranchPath(branchName);
    Utils::writeContents(branchPath, startCommit); // write branches
}

void Repository::deleteBranch(const std::string& branchName)
{
    if (!branchExists(branchName))
    {
        throw GitliteException("A branch with that name does not exist.");
    }
    if (branchName == currentBranch)
    {
        throw GitliteException("Cannot remove the current branch.");
    }

    branches.erase(branchName);
    std::string branchPath = getBranchPath(branchName);
    Utils::restrictedDelete(branchPath);
}

bool Repository::branchExists(const std::string& branchName) const {
    return Utils::exists(getBranchPath(branchName));
}

void Repository::setBranchHead(const std::string& branchName, const std::string& commitHash) {
    if (!branchExists(branchName)) {
        throw std::runtime_error("Branch does not exist: " + branchName);
    }
    branches[branchName] = commitHash;
    Utils::writeContents(getBranchPath(branchName), commitHash);
}

void Repository::setHead(const std::string& ref) {
    std::vector<std::string> headContent;
    headContent.push_back(ref);
    writeHeadFile(headContent);
}

std::string Repository::getHead() const
{
    std::string headPath = Utils::join(gitDir, "HEAD");
    if (!Utils::exists(headPath)) return "";

    std::string content = Utils::readContentsAsString(headPath);

    content.erase(std::remove(content.begin(), content.end(), '\n'), content.end()); // remove all '\n's in content
    return content;
}

std::string Repository::resolveHead() const
{
    std::string headRef = getHead();
    if (headRef.find("ref:") == 0) // HEAD points to a branch
    {
        std::string fullRef = headRef.substr(5);  // "refs/heads/master"

        size_t lastSlash = fullRef.find_last_of('/'); // master
        std::string branchName = (lastSlash != std::string::npos) ? fullRef.substr(lastSlash + 1) : fullRef;
        
        return getBranchHead(branchName);
    }
    else // Head directly points to a commit (or empty)
    {
        return headRef;
    }
}

// operations on staging area
void Repository::stageFile(const std::string& fileName, const std::vector<unsigned char>& data)
{
    Blob blob(data);
    std::string hash = storeObject(blob);
    stagingArea.addFile(fileName, hash);
    saveStagingArea();
}

void Repository::stageFile(const std::string& fileName, const std::string& content)
{
    std::vector<unsigned char> data(content.begin(), content.end());
    stageFile(fileName, data);
}

void Repository::unstageFile(const std::string& fileName)
{
    stagingArea.deleteFile(fileName);
    saveStagingArea();
}

void Repository::clearStagingArea()
{
    stagingArea.clear();
    saveStagingArea();
}

bool Repository::isStaged(const std::string& fileName) const {
    return stagingArea.existFile(fileName);
}

// operations on GitliteObjects
std::string Repository::storeObject(const GitliteObject& obj) const
{
    auto data = obj.serialize();
    std::string content(data.begin(), data.end());
    std::string header = obj.getType() + " " + std::to_string(content.size());

    std::vector<unsigned char> objectData;
    for (char c : header)
    {
        objectData.push_back(static_cast<unsigned char>(c));
    }
    objectData.push_back(0);
    objectData.insert(objectData.end(), data.begin(), data.end());
    
    std::string hash = Utils::sha1(objectData);
    std::string objectPath = getObjectPath(hash);
    
    if (!Utils::exists(objectPath))
    {
        Utils::writeContents(objectPath, objectData);
    }
    
    return hash;
}

std::unique_ptr<GitliteObject> Repository::readObject(const std::string& hash) const // deserialize
{
    std::string objectPath = getObjectPath(hash);
    auto data = Utils::readContents(objectPath);
    std::string content(data.begin(), data.end());
    
    size_t nullPos = content.find('\0');
    std::string header = content.substr(0, nullPos);
    size_t spacePos = header.find(' ');
    
    std::string type = header.substr(0, spacePos);
    std::vector<unsigned char> objectData(data.begin() + nullPos + 1, data.end());
    
    std::unique_ptr<GitliteObject> obj;
    if (type == "blob")
    {
        obj = std::make_unique<Blob>();
    }
    else if (type == "tree")
    {
        obj = std::make_unique<Tree>();
    }
    else if (type == "commit")
    {
        obj = std::make_unique<Commit>();
    }
    obj->deserialize(objectData);
    return obj;
}

std::unique_ptr<Blob> Repository::readBlob(const std::string& hash) const
{
    auto obj = readObject(hash);
    auto blob = dynamic_cast<Blob*>(obj.get());
    obj.release();
    return std::unique_ptr<Blob>(blob);
}

std::unique_ptr<Tree> Repository::readTree(const std::string& hash) const
{
    auto obj = readObject(hash);
    auto tree = dynamic_cast<Tree*>(obj.get());
    obj.release();
    return std::unique_ptr<Tree>(tree);
}

std::unique_ptr<Commit> Repository::readCommit(const std::string& hash) const
{
    auto obj = readObject(hash);
    auto commit = dynamic_cast<Commit*>(obj.get());
    obj.release();
    return std::unique_ptr<Commit>(commit);
}

bool Repository::objectExists(const std::string& hash) const
{
    return Utils::exists(getObjectPath(hash));
}

// objections on Commit
void Repository::createInitialCommit() 
{
    Tree emptyTree;
    std::string treeHash = storeObject(emptyTree);

    std::vector<std::string> emptyFather;
    Commit initialCommit(treeHash, emptyFather, "initial commit");
    
    std::time_t initialTime = 0;
    initialCommit.setTimestamp(initialTime);
    
    std::string commitHash = storeObject(initialCommit);
    
    setBranchHead("master", commitHash);
}

std::string Repository::createCommit(const std::string& message, const std::vector<std::string>& fatherHashes)
{
    std::string treeHash = createTreeFromStaging();
    Commit commit(treeHash, fatherHashes, message);
    std::string commitHash = storeObject(commit);
    
    setBranchHead(currentBranch, commitHash); // renew the Head
    clearStagingArea();
    
    return commitHash;
}

// getter on files
std::vector<std::string> Repository::getTrackedFiles() const
{
    std::string currentCommitHash = resolveHead();
    if (currentCommitHash == "") return {};
    
    auto commit = readCommit(currentCommitHash);
    auto tree = readTree(commit->getTreeHash());
    return tree->getFileNames();
}

std::vector<std::string> Repository::getUntrackedFiles() const
{
    std::vector<std::string> untracked;
    auto trackedFiles = getTrackedFiles();
    auto allFiles = Utils::plainFilenamesIn(workTree);
    
    for (const auto& file : allFiles)
    {
        if (file == ".gitlite") continue;
        if (std::find(trackedFiles.begin(), trackedFiles.end(), file) == trackedFiles.end() && !stagingArea.existFile(file))
        {
            untracked.push_back(file);
        }
    }
    return untracked;
}

std::vector<std::string> Repository::getModifiedFiles() const
{
    std::vector<std::string> modified;
    auto trackedFiles = getTrackedFiles();
    
    for (const auto& file : trackedFiles)
    {
        std::string filePath = Utils::join(workTree, file);
        if (!Utils::exists(filePath))
        {
            modified.push_back(file);
            continue;
        }

        auto currentCommitHash = resolveHead();
        auto commit = readCommit(currentCommitHash);
        auto tree = readTree(commit->getTreeHash());
        
        if (tree->existFile(file)) {
            std::string blobHash = tree->getFileHash(file);
            auto committedBlob = readBlob(blobHash);
            auto committedContent = committedBlob->getContent();
            auto currentContent = Utils::readContents(filePath);
            
            if (committedContent != currentContent) {
                modified.push_back(file);
            }
        }
    }
    return modified;
}

bool Repository::isTracked(const std::string& fileName) const
{
    auto trackedFiles = getTrackedFiles();
    return std::find(trackedFiles.begin(), trackedFiles.end(), fileName) != trackedFiles.end();
}

// operations on files marked for removal
bool Repository::hasRmTag(const std::string& fileName) const
{
    return rmFiles.find(fileName) != rmFiles.end();
}

void Repository::addRmTag(const std::string& fileName)
{
    rmFiles.insert(fileName);
    saveRmFiles();
}

void Repository::deleteRmTag(const std::string& fileName)
{
    rmFiles.erase(fileName);
    saveRmFiles();
}

void Repository::clearAllRmTag()
{
    rmFiles.clear();
    saveRmFiles();
}

// private
std::string Repository::createTreeFromStaging() const {
    Tree tree;
    
    // 复制当前提交的tree内容
    try {
        std::string currentCommitHash = resolveHead();
        if (!currentCommitHash.empty()) {
            auto commit = readCommit(currentCommitHash);
            auto currentTree = readTree(commit->getTreeHash());
            
            auto allEntries = currentTree->getAllEntries();
            for (const auto& entry : allEntries) {
                tree.addFile(entry.first, entry.second);
            }
        }
    } catch (...) {
        // 从空tree开始
    }
    
    // 应用暂存区的更改
    auto stagedEntries = stagingArea.getAllEntries();
    for (const auto& entry : stagedEntries) {
        if (entry.second.empty()) {
            tree.deleteFile(entry.first);
        } else {
            tree.addFile(entry.first, entry.second);
        }
    }
    
    return storeObject(tree);
}

std::string Repository::getObjectPath(const std::string& hash) const
{
    if (hash.length() < 2) throw std::invalid_argument("Invalid hash");
    std::string dirName = hash.substr(0, 2);
    std::string fileName = hash.substr(2);
    return Utils::join(gitDir, "objects", dirName, fileName);
}

std::string Repository::getBranchPath(const std::string& branchName) const
{
    return Utils::join(gitDir, "refs", "heads", branchName);
}

void Repository::writeHeadFile(const std::vector<std::string>& lines)
{
    std::string headPath = Utils::join(gitDir, "HEAD");
    if (!lines.empty())
    {
        Utils::writeContents(headPath, lines[0]);
    }
}