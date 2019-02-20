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

#include "defines.hpp"
#include "glm/glm.hpp"

struct Vertex_T
{
    glm::vec3 pos;
    glm::vec2 uv;
};

struct Vertex
{
    glm::vec3 pos;
    basic::Color color;
    glm::vec2 uv;
};

using VertexBufferT = basic::Vector< Vertex_T >;
using VertexBuffer = basic::Vector< Vertex >;
using IndexBuffer = basic::Vector< basic::uint16 >;

struct Mesh
{
    VertexBuffer vb;
    IndexBuffer ib;
};

struct RenderNode
{
    basic::uint32 array_object = 0;
    basic::uint32 vertex_object = 0;
    basic::uint32 index_object = 0;
    basic::int32 elements = 0;

    ICamera* camera = nullptr;
    Material* material = nullptr;
    Transform* transform = nullptr;
    basic::Color color;

    basic::Vector<RenderNode*> children;
};

bool load_mesh( const char* file, Mesh& out_mesh );

struct RenderNodeConfig
{
    basic::String camera_name;
    basic::String shader_name;
    basic::String texture_name;
    basic::Color color;
};


RenderNode *create_node( ShaderProgram* program, Texture* texture );

void remove_node( RenderNode* node );

void init_node( RenderNode* node, VertexBuffer* vertices, IndexBuffer* indices );

void init_node( RenderNode* node, VertexBufferT* vertices, IndexBuffer* indices );

template< typename VertexType >
void update_vertices( RenderNode *node, basic::Vector<VertexType> *vertices )
{
    ASSERT( node != nullptr );
    ASSERT( vertices != nullptr );
    ASSERT( node->vertex_object > 0 );

    glBindBuffer( GL_ARRAY_BUFFER, node->vertex_object );
    glBufferData( GL_ARRAY_BUFFER,
                  sizeof( VertexType ) * vertices->get_size( ),
                  vertices->get_raw( ),
                  GL_STATIC_DRAW );
}

void update_indices( RenderNode* node, IndexBuffer* indices );

void draw_node( RenderNode* node );


