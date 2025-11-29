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