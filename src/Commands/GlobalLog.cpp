#include "../../include/Commands/GlobalLog.h"
#include <iostream>
#include <iomanip>

int Commands::GlobalLog::execute()
{
    if (!repo.isInitialized())
    {
        Utils::exitWithMessage("Not in an initialized Gitlite directory.");
    }

    std::string objectsDir = Utils::join(repo.getGitliteDir(), "objects");
    std::vector<std::string> subDirs = Utils::dirnamesIn(objectsDir); // Hint: search all subDirs under "objects"

    for (const auto& subDir : subDirs)
    {
        if (subDir.length() != 2) continue; // avoid something like ..HEAD, ..REMOVE, ..index
        std::string subDirPath = Utils::join(objectsDir, subDir);

        std::vector<std::string> files = Utils::plainFilenamesIn(subDirPath); // files under certain subDir
        for (const auto& file : files)
        {
            std::string commitHash = subDir + file; // 2 + 38 = 40, form the full commit hash
            if (commitHash.length() != 40) continue; // further check it
            //std:: cout << commitHash << std::endl;
            printCommit(commitHash);
        }
    }

    return 0;
}

void Commands::GlobalLog::printCommit(const std::string& commitHash) // print a certain Commit (hash, merge, date, message)
{
    auto commit = repo.readCommit(commitHash);
    if (!commit) return;
    
    std::cout << "===" << std::endl;
    std::cout << "commit " << commitHash << std::endl;
    std::cout << "Date: " << commit->getTime() << std::endl;
    std::cout << commit->getMessage() << std::endl;
    std::cout << std::endl;  
}