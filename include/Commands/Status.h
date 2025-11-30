#ifndef STATUS_H
#define STATUS_H

#include "../GitliteException.h"
#include "../Utils.h"
#include "../Repository.h"

namespace Commands
{
    class Status
    {
    private:
        Repository& repo;

    public:
        Status(Repository& repository) : repo(repository) {}
        int execute();
    };
}

#endif // STATUS_H