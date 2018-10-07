#pragma once

#include <cstddef>

namespace basic
{

using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;

struct Color
{
    uint8 red;
    uint8 green;
    uint8 blue;
    uint8 alpha;
};

}
