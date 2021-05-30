#pragma once

#include "types.hpp"
#include "vector.hpp"

#include <vector>

namespace basic
{

enum class ColorComponents
{
    RGB,
    RGBA,
    BGR,
};

struct Image
{
    uint32 width;
    uint32 height;
    uint32 components;

    std::vector<uint8> data;
};

bool load_image( std::vector<uint8_t> data, Image& out_image );

}
