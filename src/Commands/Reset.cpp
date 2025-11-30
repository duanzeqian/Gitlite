#include "../../include/Commands/Reset.h"
#include "../../include/Commands/Checkout.h"

int Commands::Reset::execute(const std::string& commitHash)
{
    if (!repo.isInitialized())
    {
        Utils::exitWithMessage("Not in an initialized Gitlite directory.");
    }
    if (!repo.objectExists(commitHash))
    {
        Utils::exitWithMessage("No commit with that id exists.");
    }

    // copy paste of CheckoutBranch (in Checkout.cpp)
    auto commit = repo.readCommit(commitHash);
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

    std::string currentBranch = repo.getCurrentBranch();
    repo.setBranchHead(currentBranch, commitHash); // change the head

    return 0;
}