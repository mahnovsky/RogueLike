#include "basic/time.hpp"

#include <windows.h>

namespace basic
{

uint64 get_milliseconds()
{
    uint64 milliseconds = GetTickCount64();

    return milliseconds;
}

}
