#ifndef ADD_H
#define ADD_H

#include "../GitliteException.h"
#include "../Utils.h"
#include "../Blob.h"
#include "../Tree.h"
#include "../Commit.h"
#include "../GitliteObject.h"
#include "../Repository.h"
#include "Helper/Touched.h"

namespace Commands
{
    class Add
    {
    public:
        Add(Repository& repository) : repo(repository) {}
        int execute(const std::string& fileName);
        
    private:
        static bool fileContentMatchesCommit(const std::string& fileName, 
                                           const std::string& commitHash,
                                           Repository& repo);
        static std::string getCurrentCommitHash(Repository& repo);
        Repository& repo;
    };
}

#endif // ADD_H