#pragma once

#include "types.hpp"
#include "vector.hpp"

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
    ColorComponents components; 

    Vector<char> data;
};

bool load_image( Vector<char> data, Image& out_image );

}
