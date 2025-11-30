#ifndef FIND_H
#define FIND_H

#include "../GitliteException.h"
#include "../Utils.h"
#include "../Repository.h"

namespace Commands
{
    class Find
    {
    private:
        Repository& repo;
        bool exists = false;

    public:
        Find(Repository& repository) : repo(repository) {}
        int execute(const std::string& message);
        bool messageExists() const;
        void findMessage();
        void resetStatus();

    private:
        void findCommit(const std::string& commitHash, const std::string& message);
    };
}

#endif // FIND_H