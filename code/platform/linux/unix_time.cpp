#include "basic/time.hpp"

#include <sys/time.h>

namespace basic
{

double get_milliseconds()
{
    struct timeval te; 
    gettimeofday(&te, nullptr);
    double milliseconds = static_cast<double>(te.tv_sec) * 1000.0 +
            static_cast<double>(te.tv_usec) / 1000;

    return milliseconds;
}

}
