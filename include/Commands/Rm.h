#ifndef RM_H
#define RM_H

#include "../Repository.h"

namespace Commands
{
    class RM
    {
    private:
        Repository& repo;
    public:
        Rm(Repository& repository) : repo(repository) {}
        int execute(const std::string& fileName);
    }
}

#endif // RM_H