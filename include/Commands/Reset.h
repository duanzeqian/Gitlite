#ifndef RESET_H
#define RESET_H

#include "../GitliteException.h"
#include "../Utils.h"
#include "../Repository.h"

namespace Commands
{
    class Reset
    {
    private:
        Repository& repo;
        
    public:
        Reset(Repository& repository) : repo(repository) {}
        int execute(const std::string& commitHash);
    };
}

#endif // RESET_H