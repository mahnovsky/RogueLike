#pragma once

#include "basic/vector.hpp"

struct Point
{
    float x;
    float y;
    float z;
};

struct Vertex
{
    Point pos;
    basic::Color color;    
};


using VertexBuffer = basic::Vector<Vertex>; 
using IndexBuffer = basic::Vector<basic::uint16>;

