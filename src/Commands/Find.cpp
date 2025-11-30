#include "../../include/Commands/Find.h"

int Commands::Find::execute(const std::string& message)
{
    if (!repo.isInitialized())
    {
        Utils::exitWithMessage("Not in an initialized Gitlite directory.");
    }

    std::string objectsDir = Utils::join(repo.getGitliteDir(), "objects");
    std::vector<std::string> subDirs = Utils::dirnamesIn(objectsDir); // Hint: search all subDirs under "objects"

    for (const auto& subDir : subDirs)
    {
        if (subDir.length() != 2) continue; // avoid something like ..HEAD, ..REMOVE, ..index
        std::string subDirPath = Utils::join(objectsDir, subDir);

        std::vector<std::string> files = Utils::plainFilenamesIn(subDirPath); // files under certain subDir
        for (const auto& file : files)
        {
            std::string commitHash = subDir + file; // 2 + 38 = 40, form the full commit hash
            if (commitHash.length() != 40) continue; // further check it
            //std:: cout << commitHash << std::endl;
            findCommit(commitHash, message);
        }
    }

    if (!messageExists())
    {
        Utils::exitWithMessage("Found no commit with that message.");
    }

    resetStatus();
    return 0;
}

void Commands::Find::findCommit(const std::string& commitHash, const std::string& message) // pretty like Global-log
{
    auto commit = repo.readCommit(commitHash);
    if (!commit) return;
    
    if (commit->getMessage() == message)
    {
        std::cout << commitHash << std::endl;
        findMessage();
    }
}

bool Commands::Find::messageExists() const { return exists; }
void Commands::Find::findMessage() { exists = true; }
void Commands::Find::resetStatus() { exists = false; }