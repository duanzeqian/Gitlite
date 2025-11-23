#ifndef INIT_H
#define INIT_H

#include "../Command.h"
#include "../GitliteException.h"

class Init : public Command
{
public:
    int execute(const std::vector<std::string>& args) override;
    std::string getName() const override { return "init"; }
    
private:
    void createInitialCommit(const std::string& gitDir);
    void createDirectoryStructure(const std::string& gitDir);
    void createInitialFiles(const std::string& gitDir);
    bool isValidTimestamp(std::time_t timestamp);
};

#endif // INIT_H