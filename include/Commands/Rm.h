#ifndef RM_H
#define RM_H

#include "../GitliteException.h"
#include "../Utils.h"
#include "../Repository.h"

namespace Commands
{
    class Rm
    {
    private:
        Repository& repo;
        
    public:
        Rm(Repository& repository) : repo(repository) {}
        int execute(const std::string& fileName);
    };
}

#endif // RM_H