#pragma once

#include "basic/vector.hpp"


struct Vertex
{
    float x;
    float y;
    float z;
    basic::uint8 r;
    basic::uint8 g;
    basic::uint8 b;
    basic::uint8 a;
};


using VertexBuffer = basic::Vector<Vertex>; 
using IndexBuffer = basic::Vector<basic::uint16>;

