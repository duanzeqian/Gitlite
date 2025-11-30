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

    // waiting to be realized in bonus
    std::cout << "=== Modifications Not Staged For Commit ===" << std::endl;
    std::cout << std::endl;

    std::cout << "=== Untracked Files ===" << std::endl;
    return 0;
}