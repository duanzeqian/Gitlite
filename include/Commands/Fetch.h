#ifndef FETCH_H
#define FETCH_H

#include "../GitliteException.h"
#include "../Utils.h"
#include "../Repository.h"

namespace Commands
{
    class Fetch
    {
    private:
        Repository& repo;

    public:
        Fetch(Repository& repository) : repo(repository) {}
        int execute(const std::string& remoteName, const std::string& remoteBranchName);
    };
}

#endif // FETCH_H