#include "../include/Commit.h"
#include "../include/Utils.h"
#include <sstream>
#include <iomanip>
#include <ctime>

Commit::Commit() : timestamp(std::time(nullptr)) {}

Commit::Commit(const std::string& treeHash, 
               const std::vector<std::string>& fatherHashes,
               const std::string& message,
               std::time_t timestamp)
    : treeHash(treeHash),
      fatherHashes(fatherHashes),
      message(message),
      timestamp(timestamp) 
{}

std::string Commit::getType() const 
{
    return "commit";
}

std::vector<unsigned char> Commit::serialize() const 
{
    std::stringstream data;
    // treeHash \n size \n fatherHash1 \n ... fatherhashn \n timestamp \n message

    data << treeHash << "\n" << fatherHashes.size() << "\n";
    
    for (const auto& father : fatherHashes)
    {
        data << father << "\n";
    }
    
    data << timestamp << "\n" << message;
    
    std::string dataStr = data.str();
    return std::vector<unsigned char>(dataStr.begin(), dataStr.end());
}

void Commit::deserialize(const std::vector<unsigned char>& data) 
{
    std::string dataStr(data.begin(), data.end());
    std::istringstream stream(dataStr);
    std::string line;
    
    fatherHashes.clear();
    
    std::getline(stream, line);
    treeHash = line;
    
    std::getline(stream, line);
    size_t fathercnt = std::stoul(line);
    
    for (size_t i = 0; i < fathercnt; ++i)
    {
        std::getline(stream, line);
        fatherHashes.push_back(line);
    }
    
    std::getline(stream, line);
    timestamp = std::stol(line);

    std::getline(stream, message);
}

std::string Commit::getTreeHash() const { return treeHash; }
std::vector<std::string> Commit::getFatherHashes() const { return fatherHashes; }
std::string Commit::getMessage() const { return message; }
std::time_t Commit::getTimestamp() const { return timestamp; }

void Commit::setTreeHash(const std::string& hash) { treeHash = hash; }
void Commit::addFatherHash(const std::string& fatherHash) { fatherHashes.push_back(fatherHash); }
void Commit::setFatherHashes(const std::vector<std::string>& fathers) { fatherHashes = fathers; }
void Commit::setMessage(const std::string& msg) { message = msg; }
void Commit::setTimestamp(std::time_t time) { timestamp = time; }

bool Commit::isMergeCommit() const 
{ 
    return fatherHashes.size() >= 2;//have more than 1 fathercommit (actually 2) -> merge commit
}
bool Commit::isInitialCommit() const 
{ 
    return fatherHashes.empty();//have no fathercommit -> initial commit
}
std::string Commit::getShortMessage() const 
{
    size_t pos = message.find('\n');
    return (pos == std::string::npos) ? message : message.substr(0, pos);
}
std::string Commit::getTime() const 
{
    //"EEE MMM dd HH:mm:ss yyyy Z"
    std::tm* tm = std::localtime(&timestamp);//local time rather than UTC
    char buffer[80];

    //https://cplusplus.com/reference/ctime/strftime/
    //Date: Sat Nov 11 12:30:00 2017 -0800
    std::strftime(buffer, sizeof(buffer), "%c %z", tm);
    return std::string(buffer);
}

std::string Commit::getMergeFather() const //The first 7 indexes of every father commit
{
    std::string result = "";
    for (size_t i = 0; i < fatherHashes.size(); ++i)
    {
        if (i > 0) result += " ";
        result += fatherHashes[i].substr(0, 7);
    }
    return result;
}