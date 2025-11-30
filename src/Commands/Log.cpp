#include "../../include/Commands/Log.h"
#include <iostream>
#include <iomanip>
#include <ctime>

int Commands::Log::execute()
{
    if (!repo.isInitialized())
    {
        Utils::exitWithMessage("Not in an initialized Gitlite directory.");
    }
    
    std::string commitHash = repo.resolveHead();
    while (commitHash != "")
    {
        printCommit(commitHash);
        
        auto currentCommit = repo.readCommit(commitHash);
        auto fatherHashes = currentCommit->getFatherHashes();
        if (fatherHashes.empty()) break; // initial commit
        commitHash = fatherHashes[0]; // using the first father commit and overlook the other
    }
    
    return 0;
}

void Commands::Log::printCommit(const std::string& commitHash) // print a certain Commit (hash, merge, date, message)
{
    auto commit = repo.readCommit(commitHash);
    
    std::cout << "===" << std::endl;
    std::cout << "commit " << commitHash << std::endl;
    
    if (commit->isMergeCommit())
    {
        auto fatherHashes = commit->getFatherHashes();
        std::string firstFather = fatherHashes[0].substr(0, 7);
        std::string secondFather = fatherHashes[1].substr(0, 7);
        std::cout << "Merge: " << firstFather << " " << secondFather << std::endl;
    }
    
    std::cout << "Date: " << commit->getTime() << std::endl;
    std::cout << commit->getMessage() << std::endl;
    std::cout << std::endl;  
}