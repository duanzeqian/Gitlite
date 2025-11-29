#include "../../include/Commands/Add.h"
#include <filesystem>

int Commands::Add::execute(const std::string& fileName)
{
    touched::set(); // have touched

    if (!repo.isInitialized()) {
        Utils::exitWithMessage("Not in an initialized Gitlite directory.");
    }

    std::string filepath = Utils::join(".", fileName);
    if (!Utils::exists(filepath) || !Utils::isFile(filepath)) {
        Utils::exitWithMessage("File does not exist.");
    }

    /* 1. 读取工作区内容 */
    std::vector<unsigned char> current = Utils::readContents(filepath);

    /* 2. 与当前提交里的同文件比较 */
    std::string headHash = Commands::Add::getCurrentCommitHash(repo);
    if (!headHash.empty() &&
        Commands::Add::fileContentMatchesCommit(fileName, headHash, repo))
    {
        /* 内容完全一样：把它从暂存区拿掉（如果曾经暂存过） */
        repo.unstageFile(fileName);          // 确保不在 index 里
        return 0;                            // 直接结束，不再 stage
    }

    /* 3. 真正需要暂存 */
    repo.stageFile(fileName, current);
    return 0;
}

bool Commands::Add::fileContentMatchesCommit(const std::string& fileName, 
                                           const std::string& commitHash,
                                           Repository& repo) 
{
    // 读取提交对象
    std::unique_ptr<Commit> commit = repo.readCommit(commitHash);
    if (!commit) return false;
    
    // 读取树对象
    std::unique_ptr<Tree> tree = repo.readTree(commit->getTreeHash());
    if (!tree) return false;
    
    // 检查文件是否在树中
    if (!tree->existFile(fileName)) return false;
    
    // 读取提交中的blob
    std::string blobHash = tree->getFileHash(fileName);
    std::unique_ptr<Blob> committedBlob = repo.readBlob(blobHash);
    if (!committedBlob) return false;
    
    // 读取当前文件内容
    std::vector<unsigned char> currentContent = Utils::readContents(fileName);
    
    // 比较内容
    std::vector<unsigned char> committedContent = committedBlob->getContent();
    
    return currentContent == committedContent;
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