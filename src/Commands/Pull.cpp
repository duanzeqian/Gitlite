#include "../../include/Commands/Pull.h"
#include "../../include/SomeObj.h"

int Commands::Pull::execute(const std::string& remoteName, const std::string& remoteBranchName)
{
    SomeObj bloop(repo);
    bloop.fetch(remoteName, remoteBranchName);
    bloop.merge(remoteName + "/" + remoteBranchName);
    return 0;
}