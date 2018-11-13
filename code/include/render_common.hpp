#pragma once

extern "C" {
#define GLEW_STATIC
#include <GL/glew.h>
}

#ifdef __WINDOWS__
#include <GL/GL.h>
#include <GL/wglew.h>
#include <windows.h>
#else
#include <GL/gl.h>
#endif

#include "basic/vector.hpp"
#include "glm/glm.hpp"

struct Vertex
{
    glm::vec3 pos;
    basic::Color color;
    glm::vec2 uv;
};

using VertexBuffer = basic::Vector< Vertex >;
using IndexBuffer = basic::Vector< basic::uint16 >;

struct Mesh
{
    VertexBuffer vb;
    IndexBuffer ib;
};

bool load_mesh( const char* file, Mesh& out_mesh );
