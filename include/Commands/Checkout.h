#ifndef CHECKOUT_H
#define CHECKOUT_H

#include "../GitliteException.h"
#include "../Utils.h"
#include "../Repository.h"

namespace Commands
{
    class CheckoutFile
    {
    private:
        Repository& repo;
    
    public:
        CheckoutFile(Repository& repository) : repo(repository) {}
        int execute(const std::string commitHash, const std::string fileName);
    };

    /*class CheckoutBranch
    {
    private:
        Repository& repo;

    public:
        CheckoutBranch(Repository& repository) : repo(repository) {}
        int execute(const std::string branchName);
    };*/
}

#endif // CHECKOUT_H