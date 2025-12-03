#include "../../include/Commands/Status.h"
#include <iostream>

int Commands::Status::execute()
{
    if (!repo.isInitialized())
    {
        Utils::exitWithMessage("Not in an initialized Gitlite directory.");
    }

    std::cout << "=== Branches ===" << std::endl;
    auto allBranches = repo.getAllBranches();
    for (const auto& branch : allBranches)
    {
        if (branch == repo.getCurrentBranch()) std::cout << "*"; // current branch
        std::cout << branch << std::endl;
    }
    std::cout << std::endl;


    std::cout << "=== Staged Files ===" << std::endl;
    Tree stagingArea = repo.getStagingArea();
    auto stagedFiles = stagingArea.getFileNames();
    for (const auto& fileName : stagedFiles)
    {
        std::cout << fileName << std::endl;
    }
    std::cout << std::endl;


    std::cout << "=== Removed Files ===" << std::endl;
    auto rmFiles = repo.getRmFiles();
    for (const auto& fileName : rmFiles)
    {
        std::cout << fileName << std::endl;
    }
    std::cout << std::endl;


    std::cout << "=== Modifications Not Staged For Commit ===" << std::endl;
    std::vector<std::string> filesNotStaged;
    auto commit = repo.resolveHead();
    auto commitTree = repo.readTree(repo.readCommit(commit)->getTreeHash());
    auto commitFiles = commitTree->getAllEntries();

    for (const auto& entry : commitFiles)
    {
        std::string fileName = entry.first;
        std::string commitContent = repo.getCommitFileContent(fileName, commit);
        std::string currentContent = repo.getWorkTreeFileContent(fileName);
        if (commitContent != currentContent)
        {
            std::stringstream file;
            file << fileName << " ";
            if (!repo.isInWorkTree(fileName)) // Case 4: deleted
            {
                if (!repo.hasRmTag(fileName))
                {
                    file << "(deleted)";
                    filesNotStaged.push_back(file.str());
                }
            }
            else // Case 1
            {
                file << "(modified)";
                filesNotStaged.push_back(file.str());
            }
        }
    }

    for (auto& fileName : stagedFiles) // Case 2 & 3
    {
        std::string stagedContent = repo.getStagedFileContent(fileName);
        std::string currentContent = repo.getWorkTreeFileContent(fileName);
        if (stagedContent != currentContent)
        {
            std::stringstream file;
            file << fileName << " ";
            if (!repo.isInWorkTree(fileName)) file << "(deleted)";
            else file << "(modified)";
            filesNotStaged.push_back(file.str());
        }
    }

    std::sort(filesNotStaged.begin(), filesNotStaged.end());
    for (const auto& file : filesNotStaged)
    {
        std::cout << file << std::endl;
    }
    std::cout << std::endl;


    std::cout << "=== Untracked Files ===" << std::endl;
    auto untrackedFiles = repo.getUntrackedFiles();
    for (const auto& file : untrackedFiles)
    {
        std::cout << file << std::endl;
    }
    std::cout << std::endl;
    return 0;
}