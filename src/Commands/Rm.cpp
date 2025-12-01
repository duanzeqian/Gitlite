#include "../../include/Commands/Rm.h"
#include <filesystem>

int Commands::Rm::execute(const std::string& fileName)
{
    if (!repo.isInitialized())
    {
        Utils::exitWithMessage("Not in an initialized Gitlite directory.");
    }

    std::string filepath = Utils::join(".", fileName);
    bool fileExists = Utils::exists(filepath) && Utils::isFile(filepath); // whether file exists in work tree
    bool isStaged = repo.isStaged(fileName);
    bool isTracked = repo.isTracked(fileName);

    if (isTracked) // Case2: tagged for removal
    {
        repo.addRmTag(fileName);
        if (fileExists) std::remove(filepath.c_str()); // delete file in working tree
    }
    else if (isStaged) // Case1: staged but not tracked, unstage it
    {
        repo.unstageFile(fileName);
    }
    else // Case3: not staged and not tracked
    {
        Utils::exitWithMessage("No reason to remove the file.");
    }

    return 0;
}