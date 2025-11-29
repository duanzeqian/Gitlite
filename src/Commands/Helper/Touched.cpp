#include "../../../include/Commands/Helper/Touched.h"
#include "../../../include/Utils.h"
#include <fstream>

static const char* FLAG_FILE = ".gitlite/Touched.flag";

void touched::set()
{
    std::ofstream os(FLAG_FILE);
}
bool touched::get()
{
    return Utils::exists(FLAG_FILE);
}
void touched::reset()
{
    std::remove(FLAG_FILE);
}