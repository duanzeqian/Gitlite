#include "../include/Repository.h"
#include "../include/Utils.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <queue>

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
    std::string rmFilePath = Utils::join(gitDir, "remove");
    std::string content;
    for (const auto& file : rmFiles)
    {
        content += file + "\n";
    }
    Utils::writeContents(rmFilePath, content);
}
void Repository::loadRmFiles()
{
    std::string rmFilePath = Utils::join(gitDir, "remove");
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
void Repository::loadRemotes()
{
    std::string remotesDir = Utils::join(gitDir, "remotes");
    if (Utils::exists(remotesDir))
    {
        auto remoteFiles = Utils::plainFilenamesIn(remotesDir);
        for (const auto& remoteFile : remoteFiles)
        {
            std::string remotePath = Utils::join(remotesDir, remoteFile);
            if (Utils::isFile(remotePath))
            {
                std::string path = Utils::readContentsAsString(remotePath);
                remotes[remoteFile] = Remote(remoteFile, path);
            }
        }
    }
}
void Repository::saveRemote(const std::string& name, const std::string& path)
{
    std::string remotesDir = Utils::join(gitDir, "remotes");
    std::string remoteFile = Utils::join(remotesDir, name);
    Utils::writeContents(remoteFile, path);
}
void Repository::removeRemote(const std::string& name)
{
    std::string remotesDir = Utils::join(gitDir, "remotes");
    std::string remoteFile = Utils::join(remotesDir, name);
    if (Utils::exists(remoteFile))
    {
        std::remove(remoteFile.c_str());
    }
}

Repository::Repository() 
    : workTree("."), gitDir(".gitlite")
{
    loadStagingArea();
    loadRmFiles();
    loadRemotes();
}

Repository::Repository(const std::string& repoPath)
    : gitDir(repoPath)
{
    size_t pos = repoPath.find_last_of("/\\");
    if (pos != std::string::npos)
    {
        workTree = repoPath.substr(0, pos);
    }
    else
    {
        workTree = ".";
    }
    
    loadStagingArea();
    loadRmFiles();
    loadRemotes();
}

void Repository::initialize()
{    
    Utils::createDirectories(Utils::join(gitDir, "objects"));
    Utils::createDirectories(Utils::join(gitDir, "refs", "heads")); // initial directory structure
    Utils::createDirectories(Utils::join(gitDir, "refs", "remotes")); // store files from remote repo (fetch)
    
    createBranch("master"); // initial branch (master)
    setCurrentBranch("master");
    
    setHead("ref: refs/heads/master"); // initial head (towards master)

    createInitialCommit(); // initial commit
}

bool Repository::isInitialized() const
{
    return Utils::exists(gitDir) && Utils::isDirectory(gitDir);
}

// getters
std::string Repository::getGitliteDir() const { return gitDir; }
std::string Repository::getWorkTree() const { return workTree; }
std::string Repository::getCurrentBranch() const
{
    std::string headContent = getHead();
    if (headContent.find("ref: refs/heads/") == 0)
    {
        return headContent.substr(16); // delete "ref: refs/heads/"
    }
    return "";
}
std::vector<std::string> Repository::getAllBranches() const
{
    std::vector<std::string> result;
 
    std::string refsHeadsDir = Utils::join(gitDir, "refs", "heads");
    if (Utils::exists(refsHeadsDir) && Utils::isDirectory(refsHeadsDir))
    {
        auto branchFiles = Utils::plainFilenamesIn(refsHeadsDir);
        for (const auto& branchFile : branchFiles)
        {
            result.push_back(branchFile);
        }
    }
    
    std::sort(result.begin(), result.end());
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
    setHead("ref: refs/heads/" + branchName); // point Head to current branch
}

void Repository::createBranch(const std::string& branchName, const std::string& startPoint)
{
    std::string startCommit;
    if (startPoint == "") startCommit = resolveHead();
    else startCommit = startPoint;
    branches[branchName] = startCommit;
    
    std::string branchPath = getBranchPath(branchName);
    Utils::writeContents(branchPath, startCommit); // write branches
}

void Repository::deleteBranch(const std::string& branchName)
{
    branches.erase(branchName);
    std::string branchPath = getBranchPath(branchName);
    if (Utils::exists(branchPath) && Utils::isFile(branchPath))
    {
        std::remove(branchPath.c_str());
    }
}

bool Repository::branchExists(const std::string& branchName) const
{
    return Utils::exists(getBranchPath(branchName));
}

void Repository::setBranchHead(const std::string& branchName, const std::string& commitHash)
{
    branches[branchName] = commitHash;

    std::string encodedName = encodeBranchName(branchName);
    std::string headsDir = Utils::join(gitDir, "refs", "heads");
    std::string branchPath = Utils::join(headsDir, encodedName);

    Utils::writeContents(branchPath, commitHash);
}

std::string Repository::findLCA(const std::string& currentBranch, const std::string& givenBranch) const // BFS to find LCA
{
    std::string currentCommit = getBranchHead(currentBranch);
    std::unordered_set<std::string> currentAncestors;
    std::queue<std::string> q;
    q.push(currentCommit);

    while (!q.empty())
    {
        std::string commit = q.front();
        q.pop();
        if (currentAncestors.find(commit) != currentAncestors.end()) continue; // unique
        
        std::unique_ptr<Commit> commitObj;
        try
        {
            commitObj = readCommit(commit);
        }
        catch (const std::exception& e)
        {
            continue;
        }
        if (!commitObj) continue;
        
        currentAncestors.insert(commit);
        auto fatherHashes = commitObj->getFatherHashes();
        for (const auto& father : fatherHashes)
        {
            if (!father.empty())
            {
                q.push(father);
            }
        }
    }
    
    std::string givenCommit = getBranchHead(givenBranch);
    std::string result = "";
    std::unordered_set<std::string> givenAncestors;
    q = std::queue<std::string>();
    q.push(givenCommit);
    
    while (!q.empty())
    {
        std::string commit = q.front();
        q.pop();
        if (givenAncestors.find(commit) != givenAncestors.end()) continue;
        if (currentAncestors.find(commit) != currentAncestors.end()) // find LCA
        {
            result = commit;
            break;
        }
        
        std::unique_ptr<Commit> commitObj;
        try
        {
            commitObj = readCommit(commit);
        }
        catch (const std::exception& e)
        {
            continue;
        }
        if (!commitObj) continue;
        
        givenAncestors.insert(commit);
        auto fatherHashes = commitObj->getFatherHashes();
        for (const auto& father : fatherHashes)
        {
            if (!father.empty())
            {
                q.push(father);
            }
        }
    }

    return result;
}

// operations on head
void Repository::setHead(const std::string& ref)
{
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

bool Repository::isStaged(const std::string& fileName) const
{
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
    std::string treeHash = createTree();
    Commit commit(treeHash, fatherHashes, message);
    std::string commitHash = storeObject(commit);
    
    setBranchHead(getCurrentBranch(), commitHash); // renew the Head
    clearStagingArea();
    
    return commitHash;
}

std::string Repository::createCommitInMerge(const std::string& message, const std::vector<std::string>& fatherHashes, const Tree& mergedTree)
{
    std::string treeHash = createTreeInMerge(mergedTree);
    Commit commit(treeHash, fatherHashes, message);
    std::string commitHash = storeObject(commit);
    
    setBranchHead(getCurrentBranch(), commitHash); // renew the Head
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
    std::sort(untracked.begin(), untracked.end());

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

std::string Repository::getWorkTreeFileContent(std::string& fileName)
{
    std::string filepath = Utils::join(getWorkTree(), fileName);
    if (Utils::exists(filepath) && Utils::isFile(filepath))
    {
        return Utils::readContentsAsString(filepath);
    }
    return "";
}

std::string Repository::getStagedFileContent(const std::string& fileName)
{
    Tree stagingArea = getStagingArea();
    std::string blobHash = stagingArea.getFileHash(fileName);
    auto blob = readBlob(blobHash);
    if (!blob) return "";
    auto content = blob->getContent();
    return std::string(content.begin(), content.end());
}

std::string Repository::getCommitFileContent(const std::string& fileName, const std::string& commitHash)
{
    auto commit = readCommit(commitHash);
    auto tree = readTree(commit->getTreeHash());
    if (!tree->existFile(fileName))
    {
        return ""; // the file isn't in the commitHash, then return nothing
    }
    
    std::string blobHash = tree->getFileHash(fileName);
    auto blob = readBlob(blobHash);
    auto content = blob->getContent();
    return std::string(content.begin(), content.end());
}

bool Repository::isInWorkTree(const std::string& fileName) const
{
    std::string filepath = Utils::join(getWorkTree(), fileName);
    return Utils::exists(filepath) && Utils::isFile(filepath);
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

// basic operations on remote repositories
void Repository::addRemoteRepo(const std::string& repoName, const std::string& path)
{
    remotes[repoName] = Remote(repoName, path);
    saveRemote(repoName, path);
}

void Repository::deleteRemoteRepo(const std::string& repoName)
{
    remotes.erase(repoName);
    removeRemote(repoName);
}

bool Repository::remoteRepoExists(const std::string& repoName) const
{
    return remotes.find(repoName) != remotes.end();
}

Remote Repository::getRemoteRepo(const std::string& repoName) const
{
    auto it = remotes.find(repoName);
    return it->second;
}

std::vector<std::string> Repository::getAllRemoteRepos() const
{
    std::vector<std::string> result;
    for (const auto& pair : remotes)
    {
        result.push_back(pair.first);
    }
    std::sort(result.begin(), result.end());
    return result;
}

bool Repository::isAncestor(const std::string& ancestor, const std::string& descendant) const
{
    std::queue<std::string> q;
    q.push(descendant);
    std::unordered_set<std::string> visited;
    
    while (!q.empty())
    {
        std::string commit = q.front();
        q.pop();
        
        if (commit == ancestor) return true;
        if (visited.find(commit) != visited.end()) continue;
        visited.insert(commit);

        auto commitObj = readCommit(commit);
        auto parents = commitObj->getFatherHashes();
        for (const auto& parent : parents)
        {
            if (!parent.empty())
            {
                q.push(parent);
            }
        }
    }
    
    return false;
}

void Repository::copyObject(const Repository& srcRepo, const std::string& objectHash, const std::string& destRepoDir) const
{
    if (objectHash.empty()) return;
    
    std::string destObjectDir = Utils::join(destRepoDir, "objects");
    std::string destSubDir = Utils::join(destObjectDir, objectHash.substr(0, 2));
    std::string destFile = Utils::join(destSubDir, objectHash.substr(2));
    if (Utils::exists(destFile)) return;

    std::string srcFile = srcRepo.getObjectPath(objectHash);
    auto data = Utils::readContents(srcFile);
    Utils::createDirectories(destSubDir);
    Utils::writeContents(destFile, data);
}

void Repository::copyCommitHistory(const Repository& srcRepo, const std::string& commitHash, const std::string& destRepoDir) const
{
    if (commitHash.empty()) return;
    
    std::queue<std::string> toCopy;
    std::unordered_set<std::string> copied;
    
    toCopy.push(commitHash);
    
    while (!toCopy.empty())
    {
        std::string current = toCopy.front();
        toCopy.pop();
        if (copied.find(current) != copied.end()) continue;

        copyObject(srcRepo, current, destRepoDir);
        copied.insert(current);

        auto commitObj = srcRepo.readCommit(current);
        copyTree(srcRepo, commitObj->getTreeHash(), destRepoDir, copied);
        auto fathers = commitObj->getFatherHashes();
        for (const auto& father : fathers)
        {
            if (!father.empty() && copied.find(father) == copied.end())
            {
                toCopy.push(father);
            }
        }
    }
}

void Repository::copyTree(const Repository& srcRepo, const std::string& treeHash, const std::string& destRepoDir, std::unordered_set<std::string>& copied) const
{
    if (treeHash.empty() || copied.find(treeHash) != copied.end()) return;

    copyObject(srcRepo, treeHash, destRepoDir);
    copied.insert(treeHash);
    auto treeObj = srcRepo.readTree(treeHash);
    auto entries = treeObj->getAllEntries();
    
    for (const auto& entry : entries)
    {
        std::string entryHash = entry.second;
        std::string objPath = srcRepo.getObjectPath(entryHash);
        if (!Utils::exists(objPath)) continue;
        
        auto objData = Utils::readContents(objPath);
        std::string objContent(objData.begin(), objData.end());
        
        size_t nullPos = objContent.find('\0');
        if (nullPos != std::string::npos)
        {
            std::string header = objContent.substr(0, nullPos);
            if (header.find("tree") == 0) // child tree, we should reclusively copy
            {
                copyTree(srcRepo, entryHash, destRepoDir, copied);
            }
            else if (header.find("blob") == 0) // blob tree, we can directly copy
            {
                if (copied.find(entryHash) == copied.end())
                {
                    copyObject(srcRepo, entryHash, destRepoDir);
                    copied.insert(entryHash);
                }
            }
        }
    }
}

// encode name with '/'
std::string Repository::encodeBranchName(const std::string& branchName) const
{
    std::string encoded = branchName;
    std::replace(encoded.begin(), encoded.end(), '/', '~');
    return encoded;
}

std::string Repository::decodeBranchName(const std::string& encodedName) const
{
    std::string decoded = encodedName;
    std::replace(decoded.begin(), decoded.end(), '~', '/');
    return decoded;
}

// private
std::string Repository::createTree() const
{
    Tree tree;
    std::string currentCommitHash = resolveHead();
    if (currentCommitHash != "") // Tree objects of current commit
    {
        auto commit = readCommit(currentCommitHash);
        auto currentTree = readTree(commit->getTreeHash());
        auto allEntries = currentTree->getAllEntries();
        auto removedFiles = getRmFiles();
        for (const auto& entry : allEntries)
        {
            if (removedFiles.find(entry.first) == removedFiles.end()) // untrack those files removed
            {
                tree.addFile(entry.first, entry.second);
            }
        }
    }
    
    // Tree objects in staging area
    auto stagedEntries = stagingArea.getAllEntries();
    for (const auto& entry : stagedEntries)
    {
        if (entry.second.empty()) // remove
        {
            tree.deleteFile(entry.first);
        }
        else // add
        {
            tree.addFile(entry.first, entry.second);
        }
    }
    
    return storeObject(tree);
}

std::string Repository::createTreeInMerge(const Tree& mergedTree) const
{
    Tree tree = mergedTree; // father commit from mergedTree
    
    // Tree objects in staging area
    auto stagedEntries = stagingArea.getAllEntries();
    for (const auto& entry : stagedEntries)
    {
        if (entry.second.empty()) // remove
        {
            tree.deleteFile(entry.first);
        }
        else // add
        {
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