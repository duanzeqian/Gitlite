#ifndef INIT_H
#define INIT_H

#include "../Repository.h"
#include "../GitliteException.h"

namespace Commands
{
    class Init
    {
    private:
        Repository& repo;
    public:
        Init(Repository& repository) : repo(repository) {}
        int execute();
    };
}

#endif // INIT_H