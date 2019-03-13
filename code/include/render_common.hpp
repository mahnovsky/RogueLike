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

struct VertexFMT
{
    basic::uint32 offset;
    basic::uint32 size;
    basic::uint32 type;
    basic::uint32 is_normalized;
};

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

using VertexBufferP = basic::Vector< glm::vec3 >;
using VertexBufferT = basic::Vector< Vertex_T >;
using VertexBuffer = basic::Vector< Vertex >;
using IndexBuffer = basic::Vector< basic::uint16 >;

struct Mesh
{
    VertexBuffer vb;
    IndexBuffer ib;
};

enum NodeOptionFlag
{
    USE_PARENT_VAO = 1 << 1,
    USE_DYNAMIC_VBO = 1 << 2
};

struct RenderNode
{
    basic::uint32 array_object = 0;
    basic::uint32 vertex_object = 0;
    basic::int32 vertex_elements = 0;
    basic::uint32 index_object = 0;
    basic::int32 index_elements = 0;
    basic::uint32 flags;

    ICamera* camera = nullptr;
    Material* material = nullptr;
    Transform* transform = nullptr;
    basic::Color color;

    basic::Vector<RenderNode*> children;
};

bool load_mesh( const char* file, Mesh& out_mesh );

RenderNode *create_node( ShaderProgram* program, Texture* texture );

void remove_node( RenderNode* node );

basic::uint32 get_buffer_usage(RenderNode* node);

basic::uint32 create_buffer( basic::uint32 buffer_type,
                             basic::uint32 buffer_usage,
                             void* data,
                             basic::uint32 size );

basic::Vector<VertexFMT> get_fmt_list(glm::vec3*);

basic::Vector<VertexFMT> get_fmt_list(Vertex*);

basic::Vector<VertexFMT> get_fmt_list(Vertex_T*);

template<typename T>
void init_node( RenderNode *node,
                basic::Vector<T> *vertices,
                IndexBuffer *indices,
                RenderNode* parent = nullptr )
{
    ASSERT( node != nullptr );
    ASSERT( vertices != nullptr );

    node->color = basic::Color{255,255,255,255};

    if( parent && (node->flags & USE_PARENT_VAO) != 0 )
    {
        node->array_object = parent->array_object;
    }
    else
    {
        glGenVertexArrays( 1, &node->array_object );
    }

    glBindVertexArray( node->array_object );

    node->vertex_object = create_buffer( GL_ARRAY_BUFFER,
                                         get_buffer_usage( node ),
                                         vertices->get_raw(),
                                         vertices->get_size() * sizeof (T));

    node->vertex_elements = vertices->get_size();

    if( indices )
    {
        node->index_object = create_buffer( GL_ELEMENT_ARRAY_BUFFER,
                                            GL_STATIC_DRAW,
                                            indices->get_raw(),
                                            indices->get_size() * sizeof (basic::uint16) );

        node->index_elements = indices->get_size();
    }

    basic::Vector<VertexFMT> fmt_list = get_fmt_list(vertices->get_raw());

    for(basic::uint32 i = 0; i < fmt_list.get_size(); ++i)
    {
        const VertexFMT& fmt = fmt_list[i];
        glVertexAttribPointer( i, fmt.size, fmt.type, fmt.is_normalized,
                               sizeof( T ), (const void*)fmt.offset );

        glEnableVertexAttribArray( i );
    }

    glBindVertexArray( 0 );

    basic::uint32 index;
    if(parent && !parent->children.contains(node))
    {
        parent->children.push( node );
    }
}

template< typename VertexType >
void update_vertices( RenderNode *node, basic::Vector<VertexType> *vertices )
{
    ASSERT( node != nullptr );
    ASSERT( vertices != nullptr );
    ASSERT( node->vertex_object > 0 );

    glBindBuffer( GL_ARRAY_BUFFER, node->vertex_object );
    basic::uint32 size = sizeof( VertexType ) * vertices->get_size( );
    basic::int32 need_elements = static_cast<basic::int32>( vertices->get_size() );

    if( node->vertex_elements == 0 || node->vertex_elements < need_elements )
    {
        glBufferData( GL_ARRAY_BUFFER,
                      size,
                      vertices->get_raw( ),
                      get_buffer_usage( node ) );

        node->vertex_elements = need_elements;

        return;
    }

    node->vertex_elements = need_elements;

    glBufferSubData( GL_ARRAY_BUFFER, 0, size, vertices->get_raw() );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void update_indices(RenderNode* node, IndexBuffer* indices );

void draw_node(RenderNode* node, glm::mat4 *mat = nullptr, basic::uint32 prev_vao = 0);

void fill_line( const glm::vec2& p0,
                const glm::vec2& p1,
                float width,
                VertexBufferP& out_vb);

void fill_rect( const glm::vec2& left_top,
                const glm::vec2& right_bottom,
                float width,
                VertexBufferP& out_vb );

RenderNode* make_rect( ShaderProgram* shader,
                       const glm::vec2& left_top,
                       const glm::vec2& right_bottom,
                       float width = 1.f );

struct QuadGenerator
{
    QuadGenerator( const glm::vec3& size, const glm::vec2& anchor, const basic::Color& color );

    void generate(VertexBufferT &out_vb, int offset );

    void generate( IndexBuffer& out_ib, int offset );

    static IndexBuffer::Item indices[ 6 ];

private:
    glm::vec3 m_size;
    glm::vec2 m_anchor;
    basic::Color m_color;
};


