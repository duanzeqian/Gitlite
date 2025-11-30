#include "../../include/Commands/Checkout.h"

int Commands::CheckoutFile::execute(const std::string commitHash, const std::string fileName)
{
    if (!repo.isInitialized())
    {
        Utils::exitWithMessage("Not in an initialized Gitlite directory.");
    }

    std::string targetCommit = commitHash;
    if (targetCommit == "") targetCommit = repo.resolveHead(); // current head

    if (targetCommit.length() == 40 && !repo.objectExists(targetCommit)) // total
    {
        Utils::exitWithMessage("No commit with that id exists.");
    }
    else if (targetCommit.length() < 40) // abbreviation
    {
        std::string objectsDir = Utils::join(repo.getGitliteDir(), "objects");
        std::vector<std::string> subDirs = Utils::dirnamesIn(objectsDir); // Hint: search all subDirs under "objects"
        bool hasFound = false;

        for (const auto& subDir : subDirs)
        {
            if (subDir.length() != 2) continue; // avoid something like ..HEAD, ..REMOVE, ..index
            std::string subDirPath = Utils::join(objectsDir, subDir);

            std::vector<std::string> files = Utils::plainFilenamesIn(subDirPath); // files under certain subDir
            for (const auto& file : files)
            {
                std::string fullHash = subDir + file; // 2 + 38 = 40, form the full commit hash
                if (fullHash.length() != 40) continue; // further check it
                if (fullHash.substr(0, targetCommit.length()) == targetCommit)
                {
                    targetCommit = fullHash;
                    hasFound = true;
                    break;
                }
            }
            if (hasFound) break;
        }

        if (!hasFound)
        {
            Utils::exitWithMessage("No commit with that id exists.");
        }
    }

    auto commit = repo.readCommit(targetCommit);
    auto tree = repo.readTree(commit->getTreeHash());
    
    if (!tree->existFile(fileName))
    {
        Utils::exitWithMessage("File does not exist in that commit.");
    }

    std::string blobHash = tree->getFileHash(fileName);
    auto blob = repo.readBlob(blobHash);
    auto content = blob->getContent(); // get the latest content
    
    std::string filepath = Utils::join(repo.getWorkTree(), fileName);
    Utils::writeContents(filepath, content); // rewrite the content

    return 0;
}

int Commands::CheckoutBranch::execute(const std::string branchName)
{
    if (!repo.isInitialized())
    {
        Utils::exitWithMessage("Not in an initialized Gitlite directory.");
    }
    if (!repo.branchExists(branchName))
    {
        Utils::exitWithMessage("No such branch exists.");
    }
    else if (branchName == repo.getCurrentBranch())
    {
        Utils::exitWithMessage("No need to checkout the current branch.");
    }
    
    std::string targetCommit = repo.getBranchHead(branchName); 
    auto commit = repo.readCommit(targetCommit); // get the Head commit of target branch
    auto tree = repo.readTree(commit->getTreeHash());
    auto targetFiles = tree->getAllEntries();

    std::string currentCommitHash = repo.resolveHead();
    auto currentCommit = repo.readCommit(currentCommitHash); // get the Head commit of current branch
    auto currentTree = repo.readTree(currentCommit->getTreeHash());
    auto currentFiles = currentTree->getAllEntries();

    auto untrackedFiles = repo.getUntrackedFiles(); // get those unstaged files in current branch
    for (const auto& fileEntry : targetFiles) // search all files in target branch
    {
        const std::string& filename = fileEntry.first;
        if (std::find(untrackedFiles.begin(), untrackedFiles.end(), filename) != untrackedFiles.end()) // the file is unstaged
        {
            Utils::exitWithMessage("There is an untracked file in the way; delete it, or add and commit it first.");
        }
    }

    // Then we can safely operate the command "checkout" in branches
    for (const auto& fileEntry : targetFiles) // cover those files using targetFiles (either add or rewrite)
    {
        std::string blobHash = fileEntry.second;
        auto blob = repo.readBlob(blobHash);
        auto content = blob->getContent();
        
        std::string filepath = Utils::join(repo.getWorkTree(), fileEntry.first);
        Utils::writeContents(filepath, content);
    }
    for (const auto& fileEntry : currentFiles) // delete those files tracked in current branch but doesn't exist in target branch
    {
        if (targetFiles.find(fileEntry.first) == targetFiles.end())
        {
            std::string filepath = Utils::join(repo.getWorkTree(), fileEntry.first);
            if (Utils::exists(filepath))
            {
                Utils::restrictedDelete(filepath);
            }
        }
    }

    repo.clearStagingArea();
    repo.setCurrentBranch(branchName); // set the new current branch
    
    return 0;
}