#include "../../include/Commands/Checkout.h"

int Commands::CheckoutFile::execute(const std::string commitHash, const std::string fileName)
{
    if (!repo.isInitialized())
    {
        Utils::exitWithMessage("Not in an initialized Gitlite directory.");
    }

    std::string targetCommit = commitHash;
    if (targetCommit == "") targetCommit = repo.resolveHead(); // current head

    if (targetCommit.length() == 40 && !repo.objectExists(targetCommit)) // total
    {
        Utils::exitWithMessage("No commit with that id exists.");
    }
    else if (targetCommit.length() == 6) // abbreviation
    {
        std::string objectsDir = Utils::join(repo.getGitliteDir(), "objects");
        std::vector<std::string> subDirs = Utils::dirnamesIn(objectsDir); // Hint: search all subDirs under "objects"
        bool hasFound = false;

        for (const auto& subDir : subDirs)
        {
            if (subDir.length() != 2) continue; // avoid something like ..HEAD, ..REMOVE, ..index
            std::string subDirPath = Utils::join(objectsDir, subDir);

            std::vector<std::string> files = Utils::plainFilenamesIn(subDirPath); // files under certain subDir
            for (const auto& file : files)
            {
                std::string fullHash = subDir + file; // 2 + 38 = 40, form the full commit hash
                if (fullHash.length() != 40) continue; // further check it
                if (fullHash.substr(0, 6) == targetCommit)
                {
                    targetCommit = fullHash;
                    hasFound = true;
                    break;
                }
            }
            if (hasFound) break;
        }

        if (!hasFound)
        {
            Utils::exitWithMessage("No commit with that id exists.");
        }
    }

    auto commit = repo.readCommit(targetCommit);
    auto tree = repo.readTree(commit->getTreeHash());
    
    if (!tree->existFile(fileName))
    {
        Utils::exitWithMessage("File does not exist in that commit.");
    }

    std::string blobHash = tree->getFileHash(fileName);
    auto blob = repo.readBlob(blobHash);
    auto content = blob->getContent(); // get the latest content
    
    std::string filepath = Utils::join(repo.getWorkTree(), fileName);
    Utils::writeContents(filepath, content); // rewrite the content

    return 0;
}