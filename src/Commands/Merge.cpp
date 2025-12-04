#include "../../include/Commands/Merge.h"
#include "../../include/Commands/Checkout.h"
#include <vector>

int Commands::Merge::execute(const std::string& branchName)
{
    if (!repo.isInitialized())
    {
        Utils::exitWithMessage("Not in an initialized Gitlite directory.");
    }

    Tree stagingArea = repo.getStagingArea();
    auto rmFiles = repo.getRmFiles();
    std::string currentBranch = repo.getCurrentBranch();

    //std::cout << "currentBranch: " << currentBranch << std::endl << std::endl;

    if (!stagingArea.isEmpty() || !rmFiles.empty())
    {
        Utils::exitWithMessage("You have uncommitted changes.");
    }
    if (!repo.branchExists(branchName))
    {
        Utils::exitWithMessage("A branch with that name does not exist.");
    }
    if (branchName == currentBranch)
    {
        Utils::exitWithMessage("Cannot merge a branch with itself.");
    }
    
    std::string currentCommit = repo.resolveHead();
    std::string givenCommit = repo.getBranchHead(branchName);
    std::string LCA = repo.findLCA(currentBranch, branchName);
    if (LCA == givenCommit)
    {
        std::cout << "Given branch is an ancestor of the current branch." << std::endl;
        return 0;
    }
    if (LCA == currentCommit)
    {
        Commands::CheckoutBranch checkout(repo);
        checkout.execute(branchName); // gitlite checkout [branchName]
        std::cout << "Current branch fast-forwarded." << std::endl;
        return 0;
    }
    
    // Check untracked files
    auto untrackedFiles = repo.getUntrackedFiles();
    auto givenTree = repo.readTree(repo.readCommit(givenCommit)->getTreeHash());
    auto givenFiles = givenTree->getAllEntries();
    for (const auto& fileEntry : givenFiles)
    {
        if (std::find(untrackedFiles.begin(), untrackedFiles.end(), fileEntry.first) != untrackedFiles.end())
        {
            Utils::exitWithMessage("There is an untracked file in the way; delete it, or add and commit it first.");
        }
    }
    
    // Then we can merge them safely (or can we?)
    auto LCATree = repo.readTree(repo.readCommit(LCA)->getTreeHash());
    auto currentTree = repo.readTree(repo.readCommit(currentCommit)->getTreeHash());
    auto LCAFiles = LCATree->getAllEntries();
    auto currentFiles = currentTree->getAllEntries();

    Tree mergedTree;
    for (const auto& entry : currentFiles)
    {
        mergedTree.addFile(entry.first, entry.second);
    }

    bool hasConflict = false;

    std::vector<std::string> allFiles;
    for (const auto& file : LCAFiles) allFiles.push_back(file.first);
    for (const auto& file : currentFiles) allFiles.push_back(file.first);
    for (const auto& file : givenFiles) allFiles.push_back(file.first);

    std::sort(allFiles.begin(), allFiles.end());
    allFiles.erase(std::unique(allFiles.begin(), allFiles.end()), allFiles.end()); // distinct file names
    
    for (const auto& fileName : allFiles)
    {
        // whether the file is in LCA/ current branch/ given branch, and get their contents
        bool inLCA = LCATree->existFile(fileName);
        bool inCurrent = currentTree->existFile(fileName);
        bool inGiven = givenTree->existFile(fileName);
        std::string LCAContent = repo.getCommitFileContent(fileName, LCA);
        std::string currentContent = repo.getCommitFileContent(fileName, currentCommit);
        std::string givenContent = repo.getCommitFileContent(fileName, givenCommit);
        
        // the status of modified (but not deleted)
        bool modifiedInCurrent = (inLCA && inCurrent && (LCAContent != currentContent));
        bool modifiedInGiven = (inLCA && inGiven && (LCAContent != givenContent));
        
        if (inLCA && (LCAContent == currentContent) && inGiven && (LCAContent != givenContent)) // Case 1
        {
            repo.stageFile(fileName, givenContent); // stage the file as content in given branch

            std::string givenHash = (inGiven) ? givenTree->getFileHash(fileName) : "";
            mergedTree.addFile(fileName, givenHash);

            std::string filepath = Utils::join(repo.getWorkTree(), fileName); // change the workTree
            Utils::writeContents(filepath, givenContent);
        }
        else if (inLCA && inCurrent && (LCAContent != currentContent) && (LCAContent == givenContent)) // Case 2
        {
            continue;
        }
        else if ((inLCA && (LCAContent != currentContent) && currentContent == givenContent) || // Case 3-1
                (inLCA && !inCurrent && !inGiven)) // Case 3-2
        {
            continue;
        }
        else if (!inLCA && inCurrent && !inGiven) // Case 4
        {
            continue;
        }
        else if (!inLCA && inGiven && !inCurrent) // Case 5
        {
            std::string filepath = Utils::join(repo.getWorkTree(), fileName);
            Utils::writeContents(filepath, givenContent); // Step 1: checkout to workTree
            
            repo.stageFile(fileName, givenContent); // Step 2: stage it

            std::string givenHash = givenTree->getFileHash(fileName);
            mergedTree.addFile(fileName, givenHash);
        }
        else if (inLCA && (LCAContent == currentContent) && !inGiven) // Case 6
        {
            std::string filepath = Utils::join(repo.getWorkTree(), fileName);
            if (Utils::exists(filepath) && Utils::isFile(filepath))
            {
                std::remove(filepath.c_str());
            }

            mergedTree.deleteFile(fileName);
        }
        else if (inLCA && (LCAContent == givenContent) && !inCurrent) // Case 7
        {
            continue;
        }
        else if ((inLCA && (LCAContent != currentContent) && (LCAContent != givenContent) && (currentContent != givenContent)) || // Case 8-1
                (inLCA && (LCAContent != currentContent) && !inGiven) || (inLCA && (LCAContent != givenContent) && !inCurrent) || // Case 8-2
                (!inLCA && (currentContent != givenContent))) // Case 8-3
        {
            handleConflict(fileName, currentContent, givenContent);
            hasConflict = true;
        }
    }
    
    if (hasConflict) 
    {
        std::cout << "Encountered a merge conflict." << std::endl;
        //return 0;
    }

    std::string decodedName = repo.decodeBranchName(branchName);
    std::vector<std::string> fatherHashes = {currentCommit, givenCommit};
    std::string message = "Merged " + decodedName + " into " + currentBranch + "."; // CAUTION!!! "."
    repo.createCommitInMerge(message, fatherHashes, mergedTree);
    
    return 0;
}

void Commands::Merge::handleConflict(const std::string& fileName, const std::string& currentContent, const std::string& givenContent)
{
    std::stringstream conflictContent;
    conflictContent << "<<<<<<< HEAD\n";
    conflictContent << currentContent;
    conflictContent << "=======\n";
    conflictContent << givenContent;
    conflictContent << ">>>>>>>\n";
    
    std::string filepath = Utils::join(repo.getWorkTree(), fileName);
    Utils::writeContents(filepath, conflictContent.str()); // store in workTree

    repo.stageFile(fileName, conflictContent.str()); // stage it
}