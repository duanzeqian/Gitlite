#include "../../include/Commands/RmRemote.h"

int Commands::RmRemote::execute(const std::string& remoteName)
{
    if (!repo.remoteRepoExists(remoteName))
    {
        Utils::exitWithMessage("A remote with that name does not exist.");
    }

    repo.deleteRemoteRepo(remoteName);
    return 0;
}