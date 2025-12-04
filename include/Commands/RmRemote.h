#ifndef RM_REMOTE
#define RM_REMOTE

#include "../GitliteException.h"
#include "../Utils.h"
#include "../Repository.h"

namespace Commands
{
    class RmRemote
    {
    private:
        Repository& repo;
    
    public:
        RmRemote(Repository& repository) : repo(repository) {}
        int execute(const std::string& remoteName);
    };
}

#endif // RM_REMOTE