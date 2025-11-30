#ifndef SOME_OBJ_H
#define SOME_OBJ_H

#include <string>
#include "Repository.h"
#include "Commands/Init.h"
#include "Commands/Add.h"
#include "Commands/CommitCmd.h"
#include "Commands/Rm.h"
#include "Commands/Log.h"
#include "Commands/GlobalLog.h"
#include "Commands/Find.h"

class SomeObj
{
private:
    Repository& repo;
    
public:
    SomeObj(Repository& repository) : repo(repository) {}
    void init();
    void add(const std::string& fileName);
    void commit(const std::string& message);
    void rm(const std::string& fileName);
    void log();
    void globalLog();
    void find(const std::string& message);
};

#endif // SOME_OBJ_H