/*
    一切command的基类
*/
#ifndef COMMAND_H
#define COMMAND_H

#include <vector>
#include <string>
#include <memory>

class Command
{
public:
    virtual ~Command() = default;
    virtual int execute(const std::vector<std::string>& args) = 0;
    virtual std::string getName() const = 0;
};

#endif // COMMAND_H