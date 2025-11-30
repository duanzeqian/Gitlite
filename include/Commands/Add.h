#ifndef ADD_H
#define ADD_H

#include "../GitliteException.h"
#include "../Utils.h"
#include "../Blob.h"
#include "../Tree.h"
#include "../Commit.h"
#include "../GitliteObject.h"
#include "../Repository.h"

namespace Commands
{
    class Add
    {   
    private:
        Repository& repo;
        static bool fileCompare(const std::string& fileName, const std::string& commitHash, Repository& repo);
    public:
        Add(Repository& repository) : repo(repository) {}
        int execute(const std::string& fileName);
    };
}

#endif // ADD_H