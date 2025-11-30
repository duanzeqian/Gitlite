#include "../../include/Commands/Add.h"
#include <filesystem>

int Commands::Add::execute(const std::string& fileName)
{
    if (!repo.isInitialized())
    {
        Utils::exitWithMessage("Not in an initialized Gitlite directory.");
    }
    std::string filepath = Utils::join(".", fileName);
    if (!Utils::exists(filepath) || !Utils::isFile(filepath))
    {
        Utils::exitWithMessage("File does not exist.");
    }

    std::vector<unsigned char> current = Utils::readContents(filepath);

    std::string headHash = repo.resolveHead();
    
    if (headHash != "")
    {
        if (Commands::Add::fileCompare(fileName, headHash, repo))
        {
            repo.unstageFile(fileName); // remove the same file from staging area

            // Status with a removal followed by an add that restores former contents. 
            // Should simply "unremove" the file without staging.
            // for 3-status-05
            if (repo.hasRmTag(fileName))
            {
                repo.deleteRmTag(fileName);
            }
            
            return 0;
        }
    }

    

    repo.stageFile(fileName, current);
    return 0;
}

bool Commands::Add::fileCompare(const std::string& fileName, const std::string& commitHash, Repository& repo) 
{
    std::unique_ptr<Commit> commit = repo.readCommit(commitHash); // the whole commit
    if (!commit) return false;
    
    std::unique_ptr<Tree> tree = repo.readTree(commit->getTreeHash()); // Tree object
    if (!tree) return false;
    
    if (!tree->existFile(fileName)) return false; // whether the fileName is in the Tree object
    
    std::string blobHash = tree->getFileHash(fileName);
    std::unique_ptr<Blob> committedBlob = repo.readBlob(blobHash);
    if (!committedBlob) return false; // the Blob has not been committed
    
    std::string filepath = Utils::join(".", fileName);
    std::vector<unsigned char> currentContent = Utils::readContents(filepath); // get the content (newly added)
    std::vector<unsigned char> committedContent = committedBlob->getContent(); // get the content (committed)
    return currentContent == committedContent; // compare the content
}