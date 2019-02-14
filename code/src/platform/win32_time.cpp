#include "basic/time.hpp"

#include <windows.h>

#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */

namespace basic
{

double get_milliseconds()
{
    uint64 milliseconds = GetTickCount64();

	return static_cast<double>(milliseconds);
}

}
