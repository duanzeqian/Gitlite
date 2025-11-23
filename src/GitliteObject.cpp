#include "../include/GitliteObject.h"
#include "../include/Utils.h"

std::string GitliteObject::computeHash()
{
    auto data = serialize();
    std::string content(data.begin(), data.end());
    hash = Utils::sha1(getType() + " " + std::to_string(content.size()) + '\0' + content);
    return hash;
}

std::string GitliteObject::getHash() const
{
    return hash;
}

void GitliteObject::setHash(const std::string& h)
{
    hash = h;
}

size_t GitliteObject::getSize() const
{
    auto data = serialize();
    return data.size();
}