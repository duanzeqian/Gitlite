#include "../../include/Commands/AddRemote.h"

int Commands::AddRemote::execute(const std::string& remoteName, const std::string& remotePath)
{
    if (repo.remoteRepoExists(remoteName))
    {
        Utils::exitWithMessage("A remote with that name already exists.");
    }

    repo.addRemoteRepo(remoteName, remotePath);
    return 0;
}