#ifndef LOG_H
#define LOG_H

#include "../GitliteException.h"
#include "../Utils.h"
#include "../Repository.h"
#include "../Commit.h"

namespace Commands
{
    class Log
    {
    private:
        Repository& repo;
    public:
        Log(Repository& repository) : repo(repository) {}
        int execute();

    private:
        void printCommit(const std::string& commitHash);
    };
}

#endif // LOG_H