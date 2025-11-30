#include "../include/SomeObj.h"

void SomeObj::init()
{
    Commands::Init init(repo);
    init.execute();
}

void SomeObj::add(const std::string& fileName)
{
    //std::cout << "ADD - repo address: " << &repo << std::endl;
    Commands::Add add(repo);
    add.execute(fileName);
}

void SomeObj::commit(const std::string& message)
{
    //std::cout << "COMMIT - repo address: " << &repo << std::endl;
    Commands::CommitCmd commit(repo);
    commit.execute(message);
}

void SomeObj::rm(const std::string& fileName)
{
    Commands::Rm rm(repo);
    rm.execute(fileName);
}

void SomeObj::log()
{
    Commands::Log log(repo);
    log.execute();
}

void SomeObj::globalLog()
{
    Commands::GlobalLog globalLog(repo);
    globalLog.execute();
}

void SomeObj::find(const std::string& message)
{
    Commands::Find find(repo);
    find.execute(message);
}

void SomeObj::checkoutFile(const std::string& commitHash, const std::string& fileName)
{
    Commands::CheckoutFile checkoutFile(repo);
    checkoutFile.execute(commitHash, fileName);
}

void SomeObj::checkoutBranch(const std::string& branchName)
{
    Commands::CheckoutBranch checkoutBranch(repo);
    checkoutBranch.execute(branchName);
}

void SomeObj::status()
{
    Commands::Status status(repo);
    status.execute();
}

void SomeObj::branch(const std::string& branchName)
{
    Commands::Branch branch(repo);
    branch.execute(branchName);
}

void SomeObj::rmBranch(const std::string& branchName)
{
    Commands::RmBranch rmBranch(repo);
    rmBranch.execute(branchName);
}

void SomeObj::reset(const std::string& commitHash)
{
    Commands::Reset reset(repo);
    reset.execute(commitHash);
}