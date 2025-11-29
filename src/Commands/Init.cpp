#include "../../include/Commands/Init.h"

int Commands::Init::execute() 
{
    if (repo.isInitialized())
    {
        throw GitliteException("A Gitlite version-control system already exists in the current directory.");
    }
    repo.initialize(".");
    
    return 0;
}