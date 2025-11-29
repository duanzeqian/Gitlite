#include "../include/SomeObj.h"

void SomeObj::init() {
    Commands::Init init(repo);  // 使用同一个 repo
    init.execute();
}

void SomeObj::add(const std::string& fileName) {
    Commands::Add add(repo);    // 使用同一个 repo
    add.execute(fileName);
}

void SomeObj::commit(const std::string& message) {
    Commands::CommitCmd commit(repo);  // 使用同一个 repo
    commit.execute(message);
}