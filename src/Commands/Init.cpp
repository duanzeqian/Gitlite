#include "../../include/Commands/Init.h"
#include "../Utils.h"
#include "../Blob.h"
#include "../Tree.h"
#include "../Commit.h"
#include <ctime>
#include <iomanip>
#include <sstream>

int Init::execute(const std::vector<std::string>& args) 
{
    std::string currentDir = ".";
    std::string gitDir = Utils::join(currentDir, ".gitlite");
    
    if (Utils::exists(gitDir))
    {
        throw GitliteException("A Gitlite version-control system already exists in the current directory.");
    }

    createDirectoryStructure(gitDir);//main directory structure
    createInitialFiles(gitDir);//initial file
    createInitialCommit(gitDir);//initial commit
    
    //Utils::message("Initialized empty GitLite repository in " + gitDir);
    return 0;
}

void Init::createDirectoryStructure(const std::string& gitDir) 
{
    Utils::createDirectories(Utils::join(gitDir, "objects"));
    Utils::createDirectories(Utils::join(gitDir, "refs", "heads"));
    Utils::createDirectories(Utils::join(gitDir, "refs", "tags"));
}

void Init::createInitialFiles(const std::string& gitDir) 
{
    // 创建 HEAD 文件，指向 master 分支
    Utils::writeContents(Utils::join(gitDir, "HEAD"), "ref: refs/heads/master\n");
    
    // 创建 description 文件
    Utils::writeContents(Utils::join(gitDir, "description"), 
                        "Unnamed repository; edit this file to name it for gitlite.\n");
    
    // 创建 config 文件（简化版）
    std::string config = "[core]\n";
    config += "    repositoryformatversion = 0\n";
    config += "    filemode = true\n";
    config += "    bare = false\n";
    Utils::writeContents(Utils::join(gitDir, "config"), config);
}

void Init::createInitialCommit(const std::string& gitDir) 
{
    // 1. 创建空的 Tree 对象
    Tree emptyTree;
    std::string treeHash = Utils::storeGitliteObject(emptyTree);
    
    // 2. 创建初始提交
    std::vector<std::string> parentHashes;  // 空，表示初始提交
    
    Commit initialCommit(treeHash, parentHashes, "initial commit");
    
    // 设置 Unix 纪元时间戳 (January 1, 1970)
    std::time_t epoch = 0;
    initialCommit.setTimestamp(epoch);
    
    // 设置默认作者和提交者
    std::string authorInfo = "GitLite System <system@gitlite> " + std::to_string(epoch) + " +0000";
    initialCommit.setAuthor(authorInfo);
    initialCommit.setCommitter(authorInfo);
    
    // 计算并存储提交对象
    std::string commitHash = initialCommit.computeHash();
    Utils::storeGitliteObject(initialCommit);
    
    // 3. 设置 master 分支指向初始提交
    std::string masterRefPath = Utils::join(gitDir, "refs", "heads", "master");
    Utils::writeContents(masterRefPath, commitHash + "\n");
}

bool Init::isValidTimestamp(std::time_t timestamp) 
{
    return timestamp >= 0;
}