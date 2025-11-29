#ifndef COMMIT_CMD_H
#define COMMIT_CMD_H

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
    class CommitCmd
    {
    public:
        int execute(const std::string& message);
        CommitCmd(Repository& repository) : repo(repository) {}
        
    private:
        Repository& repo;
        static std::string createCommitTree(Repository& repo, const Tree& stagingArea);
    };
}

#endif // COMMIT_CMD_H