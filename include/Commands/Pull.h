#ifndef PULL_H
#define PULL_H

#include "../GitliteException.h"
#include "../Utils.h"
#include "../Repository.h"

namespace Commands
{
    class Pull
    {
    private:
        Repository& repo;

    public:
        Pull(Repository& repository) : repo(repository) {}
        int execute(const std::string& remoteName, const std::string& remoteBranchName);
    };
}

#endif // PULL_H