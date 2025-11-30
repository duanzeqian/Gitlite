#ifndef RM_BRANCH_H
#define RM_BRANCH_H

#include "../GitliteException.h"
#include "../Utils.h"
#include "../Repository.h"

namespace Commands
{
    class RmBranch
    {
    private:
        Repository& repo;

    public:
        RmBranch(Repository& repository) : repo(repository) {}
        int execute(const std::string& branchName);
    };
}

#endif // EM_BRANCH_H