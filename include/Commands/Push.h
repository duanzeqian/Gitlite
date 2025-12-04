#ifndef PUSH_H
#define PHSH_H

#include "../GitliteException.h"
#include "../Utils.h"
#include "../Repository.h"

namespace Commands
{
    class Push
    {
    private:
        Repository& repo;

    public:
        Push(Repository& repository) : repo(repository) {}
        int execute(const std::string& remoteName, const std::string& remoteBranchName);
    };
}

#endif // PUSH_H