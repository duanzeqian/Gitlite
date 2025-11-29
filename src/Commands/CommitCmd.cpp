#include "../../include/Commands/CommitCmd.h"
#include <ctime>
#include <memory>

int Commands::CommitCmd::execute(const std::string& message) 
{
    if (!repo.isInitialized())
    {
        Utils::exitWithMessage("Not in an initialized Gitlite directory.");
    }
    if (message.empty())
    {
        Utils::exitWithMessage("Please enter a commit message.");
    }
    
    Tree stagingArea = repo.getStagingArea(); // Tree Object in staging area (repo)
    if (stagingArea.isEmpty()) // No changes in staging area
    {
        Utils::exitWithMessage("No changes added to the commit."); // without being operated
    }
    
    std::string fatherCommitHash = repo.resolveHead(); // get father commit
    //std::cout << "Debug here" << std::endl;
    std::string treeHash = createCommitTree(repo, stagingArea); // has changes in staging area, then create commit tree
    std::vector<std::string> fatherHashes;
    if (fatherCommitHash != "")
    {
        fatherHashes.push_back(fatherCommitHash);
    }
    
    Commit commitObj(treeHash, fatherHashes, message); // create commit object
    std::string commitHash = repo.storeObject(commitObj);
    std::string currentBranch = repo.getCurrentBranch();
    repo.setBranchHead(currentBranch, commitHash); // Head points to the current branch
    
    repo.clearStagingArea(); // clear the staging area
    return 0;
        
}

std::string Commands::CommitCmd::createCommitTree(Repository& repo, const Tree& stagingArea) 
{
    std::string fatherTreeHash;
    std::string fatherCommitHash = repo.resolveHead();
    if (fatherCommitHash != "") // has father commit
    {
        std::unique_ptr<Commit> fatherCommit = repo.readCommit(fatherCommitHash);
        if (fatherCommit)
        {
            fatherTreeHash = fatherCommit->getTreeHash();
        }
    }

    Tree newTree;
    if (!fatherTreeHash.empty()) // Add fatherTree to Tree first
    {
        std::unique_ptr<Tree> fatherTree = repo.readTree(fatherTreeHash);
        auto entries = fatherTree->getAllEntries();
        for (const auto& entry : entries)
        {
            newTree.addFile(entry.first, entry.second);
        }
    }
    
    // Add staging area changes to Tree then
    auto stagedEntries = stagingArea.getAllEntries();
    for (const auto& entry : stagedEntries)
    {
        newTree.addFile(entry.first, entry.second);
    }

    return repo.storeObject(newTree); // treeHash
}