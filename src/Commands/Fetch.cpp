#include "../../include/Commands/Fetch.h"

int Commands::Fetch::execute(const std::string& remoteName, const std::string& remoteBranchName)
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
    std::string localRepoPath = repo.getGitliteDir();
    Repository remoteRepo(remoteRepoPath);
    if (!remoteRepo.branchExists(remoteBranchName))
    {
        Utils::exitWithMessage("That remote does not have that branch.");
    }

    std::string remoteHead = remoteRepo.getBranchHead(remoteBranchName);
    
    remoteRepo.copyCommitHistory(remoteRepo, remoteHead, localRepoPath);
    std::string localRemoteBranch = remoteName + "/" + remoteBranchName;
    repo.setBranchHead(localRemoteBranch, remoteHead);  // [remote name]/[remote branch name] 
    // e.g. R1/master (from testcase 6-remote-01)

    return 0;
}