#pragma once

#include "basic/vector.hpp"
#include "glm/glm.hpp"

struct Vertex
{
    glm::vec3 pos;
    basic::Color color;
    glm::vec2 uv;
};


using VertexBuffer = basic::Vector<Vertex>; 
using IndexBuffer = basic::Vector<basic::uint16>;

