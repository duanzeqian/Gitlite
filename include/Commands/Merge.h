#ifndef MERGE_H
#define MERGE_H

#include "../GitliteException.h"
#include "../Utils.h"
#include "../Repository.h"

namespace Commands
{
    class Merge
    {
    private:
        Repository& repo;
        void handleConflict(const std::string& fileName, const std::string& currentContent, const std::string& givenContent);

    public:
        Merge(Repository& repository) : repo(repository) {}
        int execute(const std::string& branchName);
    };
}

#endif // MERGE_H