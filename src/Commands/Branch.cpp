#include "../../include/Commands/Branch.h"

int Commands::Branch::execute(const std::string& branchName)
{
    if (!repo.isInitialized())
    {
        Utils::exitWithMessage("Not in an initialized Gitlite directory.");
    }
    if (repo.branchExists(branchName))
    {
        Utils::exitWithMessage("A branch with that name already exists.");
    }

    repo.createBranch(branchName);
    return 0;
}