#include "../../include/Commands/Push.h"

int Commands::Push::execute(const std::string& remoteName, const std::string& remoteBranchName)
{
    if (!repo.remoteRepoExists(remoteName))
    {
        Utils::exitWithMessage("Remote directory not found.");
    }
    Remote remote = repo.getRemoteRepo(remoteName);
    if (!remote.repoExists())
    {
        Utils::exitWithMessage("Remote directory not found.");
    }

    std::string remoteRepoPath = remote.getPath();
    Repository remoteRepo(remoteRepoPath);

    std::string localBranchName = repo.getCurrentBranch();
    std::string localHead = repo.getBranchHead(localBranchName);
    if (remoteRepo.branchExists(remoteBranchName))
    {
        std::string remoteHead = remoteRepo.getBranchHead(remoteBranchName);
        if (!repo.isAncestor(remoteHead, localHead))
        {
            Utils::exitWithMessage("Please pull down remote changes before pushing.");
        }
    }

    repo.copyCommitHistory(repo, localHead, remoteRepoPath);
    remoteRepo.setBranchHead(remoteBranchName, localHead);

    return 0;
}