#ifndef BRANCH_H
#define BRANCH_H

#include "../GitliteException.h"
#include "../Utils.h"
#include "../Repository.h"

namespace Commands
{
    class Branch
    {
    private:
        Repository& repo;
    
    public:
        Branch(Repository& repository) : repo(repository) {}
        int execute(const std::string& branchName);
    };
}

#endif // BRANCH_H