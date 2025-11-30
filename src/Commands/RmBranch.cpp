#include "../../include/Commands/RmBranch.h"

int Commands::RmBranch::execute(const std::string& branchName)
{
    if (!repo.isInitialized())
    {
        Utils::exitWithMessage("Not in an initialized Gitlite directory.");
    }
    if (!repo.branchExists(branchName))
    {
        Utils::exitWithMessage("A branch with that name does not exist.");
    }
    if (branchName == repo.getCurrentBranch())
    {
        Utils::exitWithMessage("Cannot remove the current branch.");
    }

    repo.deleteBranch(branchName);
    return 0;
}