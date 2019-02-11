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
    uint32 components;

    Vector<uint8> data;
};

bool load_image( Vector<uint8> data, Image& out_image );

}
