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
#include "Commands/Checkout.h"
#include "Commands/Status.h"
#include "Commands/Branch.h"
#include "Commands/RmBranch.h"
#include "Commands/Reset.h"

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
    void checkoutFile(const std::string& commitHash, const std::string& fileName);
    void checkoutBranch(const std::string& branchName);
    void status();
    void branch(const std::string& branchName);
    void rmBranch(const std::string& branchName);
    void reset(const std::string& commitHash);
};

#endif // SOME_OBJ_H