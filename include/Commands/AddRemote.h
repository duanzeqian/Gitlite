#ifndef ADD_REMOTE
#define ADD_REMOTE

#include "../GitliteException.h"
#include "../Utils.h"
#include "../Repository.h"

namespace Commands
{
    class AddRemote
    {
    private:
        Repository& repo;
    
    public:
        AddRemote(Repository& repository) : repo(repository) {}
        int execute(const std::string& remoteName, const std::string& remotePath);
    };
}

#endif // ADD_REMOTE