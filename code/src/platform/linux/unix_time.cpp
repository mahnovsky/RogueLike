#include "basic/time.hpp"

#include <sys/time.h>

namespace basic
{

uint64 get_milliseconds()
{
    struct timeval te; 
    gettimeofday(&te, NULL);
    uint64 milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000;

    return milliseconds;
}

}
