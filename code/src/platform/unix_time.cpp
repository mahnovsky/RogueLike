#include "basic/time.hpp"

#include <sys/time.h>

namespace basic
{

uint32 get_milliseconds()
{
    struct timeval te; 
    gettimeofday(&te, NULL);
    uint32 milliseconds = te.tv_sec * 1000 + te.tv_usec / 1000;

    return milliseconds;
}

}
