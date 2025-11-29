#include "../../include/Commands/Add.h"
#include <filesystem>

int Commands::Add::execute(const std::string& fileName)
{
    if (!repo.isInitialized()) {
        Utils::exitWithMessage("Not in an initialized Gitlite directory.");
    }

    std::string filepath = Utils::join(".", fileName);
    if (!Utils::exists(filepath) || !Utils::isFile(filepath)) {
        Utils::exitWithMessage("File does not exist.");
    }

    /* 首先：如果文件处于暂存状态且标记为待删除，则将其待删除状态移除 */
    // 注意：如果你的 Repository 还没有实现这些方法，可以先注释掉
    // if (repo.isFileMarkedForRemoval(fileName)) {
    //     repo.unmarkFileForRemoval(fileName);
    // }

    /* 1. 读取工作区内容 */
    std::vector<unsigned char> current = Utils::readContents(filepath);

    /* 2. 与当前提交里的同文件比较 - 只有在有提交时才比较 */
    std::string headHash = Commands::Add::getCurrentCommitHash(repo);
    
    // 如果有历史提交，且文件内容与当前提交相同
    if (!headHash.empty()) {
        if (Commands::Add::fileContentMatchesCommit(fileName, headHash, repo)) {
            /* 内容完全一样：把它从暂存区拿掉（如果曾经暂存过） */
            repo.unstageFile(fileName);
            return 0;
        }
    }

    /* 3. 真正需要暂存 - 这会覆盖暂存区中的先前条目 */
    repo.stageFile(fileName, current);
    return 0;
}

bool Commands::Add::fileContentMatchesCommit(const std::string& fileName, 
                                           const std::string& commitHash,
                                           Repository& repo) 
{
    try {
        // 读取提交对象
        std::unique_ptr<Commit> commit = repo.readCommit(commitHash);
        if (!commit) return false;
        
        // 读取树对象
        std::unique_ptr<Tree> tree = repo.readTree(commit->getTreeHash());
        if (!tree) return false;
        
        // 检查文件是否在树中
        if (!tree->existFile(fileName)) {
            // 文件不在提交中，需要暂存
            return false;
        }
        
        // 读取提交中的blob
        std::string blobHash = tree->getFileHash(fileName);
        std::unique_ptr<Blob> committedBlob = repo.readBlob(blobHash);
        if (!committedBlob) return false;
        
        // 读取当前文件内容 - 使用完整路径
        std::string filepath = Utils::join(".", fileName);
        std::vector<unsigned char> currentContent = Utils::readContents(filepath);
        
        // 比较内容
        std::vector<unsigned char> committedContent = committedBlob->getContent();
        
        return currentContent == committedContent;
    } catch (const std::exception& e) {
        // 如果任何步骤出错，认为内容不匹配，需要暂存
        return false;
    }
}

std::string Commands::Add::getCurrentCommitHash(Repository& repo) 
{
    try {
        return repo.resolveHead();
    } catch (const std::exception& e) {
        // 如果没有提交（初始状态），返回空字符串
        return "";
    }
}