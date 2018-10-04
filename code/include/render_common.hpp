#pragma once

#include "basic/util_functions.hpp"
#include "basic/vector.hpp"

using byte = unsigned char;
using word = unsigned short;

struct Vertex
{
    float x, y, z;
    byte r, g, b, a;
};


using VertexBuffer = basic::Vector<Vertex>; 
using IndexBuffer = basic::Vector<basic::uint16>;

