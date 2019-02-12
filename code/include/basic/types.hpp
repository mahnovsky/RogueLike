#pragma once

#include <cstdint>
#include <cstddef>

namespace basic
{

using int8 = char;
using int16 = short;
using int32 = long;
using int64 = long long;

using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;

struct Color
{
    uint8 red;
    uint8 green;
    uint8 blue;
    uint8 alpha;
};

}
