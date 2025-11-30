#ifndef GLOBAL_LOG_H
#define GLOBAL_LOG_H

#include "../GitliteException.h"
#include "../Utils.h"
#include "../Repository.h"
#include "../Commit.h"

namespace Commands
{
    class GlobalLog
    {
    private:
        Repository& repo;
    public:
        GlobalLog(Repository& repository) : repo(repository) {}
        int execute();

    private:
        void printCommit(const std::string& commitHash);
    };
}

#endif // GLOBAL_LOG_H