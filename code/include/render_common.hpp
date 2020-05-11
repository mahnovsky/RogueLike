#pragma once
#include <vector>

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

extern GLenum g_OpenGLError;

#define OPENGL_CHECK_FOR_ERRORS() \
	if ((g_OpenGLError = glGetError()) != GL_NO_ERROR) \
	{\
		LOG("OpenGL error 0x%X\n", (unsigned)g_OpenGLError); \
		g_OpenGLError = GL_NO_ERROR; \
	}


#define CHECKED_CALL(func, ... ) \
	func(__VA_ARGS__); \
	if ((g_OpenGLError = glGetError()) != GL_NO_ERROR) \
	{\
		LOG("OpenGL error 0x%X in func %s, line %d\n", (unsigned)g_OpenGLError, #func, __LINE__); \
		g_OpenGLError = GL_NO_ERROR; \
	}

enum class RenderResourceType
{
	StaticMesh,
	Texture,
	ShaderProgram,
	Count
};

struct VertexFMT
{
    basic::uint32 offset;
    basic::uint32 size;
    basic::uint32 type;
    basic::uint8 is_normalized;
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

struct MeshData
{
    VertexBuffer vb;
    IndexBuffer ib;
};

enum MeshLoadFlags
{
	IGNORE_UV = 0x1,
	IGNORE_NORMAL = 0x2
};

struct MeshLoadSettings
{
	basic::uint32 flags;
	glm::mat4 vertex_transform;
};

enum NodeOptionFlag
{
    USE_PARENT_VAO = 1 << 1,
    USE_DYNAMIC_VBO = 1 << 2
};

basic::uint32 create_buffer( basic::uint32 buffer_type,
                             basic::uint32 buffer_usage,
                             const void* data,
                             basic::uint32 size );

basic::Vector< VertexFMT > get_fmt_list( const glm::vec3* );

basic::Vector< VertexFMT > get_fmt_list( const Vertex* );

basic::Vector< VertexFMT > get_fmt_list( const Vertex_T* );

basic::int32 get_uniform(basic::uint32 program, const char* name);

void set_uniform(basic::uint32 program, const char* name, const glm::vec2& v);

void set_uniform(basic::uint32 program, const char* name, const basic::Color& color);

void set_uniform(basic::uint32 program, const char* name, const glm::mat4& mat);

void set_uniform(basic::uint32 program, const char* name, basic::int32 v);

struct RenderNode
{
public:
    template < typename T >
    void
    reset( basic::Vector< T >* vb, IndexBuffer* ib )
    {
        if ( array_object == 0 )
        {
            init_node( vb, ib );

            return;
        }

        update_vertices( vb );
        update_indices( ib );
    }

    void set_color( const basic::Color& color );

    basic::Color get_color( ) const;

    void set_camera( ICamera* camera );

    template < typename T >
    void
    init_node( basic::Vector< T >* vertices, IndexBuffer* indices, RenderNode* parent = nullptr )
    {
        ASSERT( vertices != nullptr );

        color = basic::Color{255, 255, 255, 255};

        if ( parent && ( flags & USE_PARENT_VAO ) != 0 )
        {
            array_object = parent->array_object;
        }
        else
        {
            glGenVertexArrays( 1, &array_object );
        }

        glBindVertexArray( array_object );

        vertex_object = create_buffer( GL_ARRAY_BUFFER,
                                       get_buffer_usage( ),
                                       vertices->get_raw( ),
                                       vertices->get_size( ) * sizeof( T ) );

        vertex_elements = vertices->get_size( );

        if ( indices )
        {
            index_object = create_buffer( GL_ELEMENT_ARRAY_BUFFER,
                                          GL_STATIC_DRAW,
                                          indices->get_raw( ),
                                          indices->get_size( ) * sizeof( basic::uint16 ) );

            index_elements = indices->get_size( );
        }

        basic::Vector< VertexFMT > fmt_list = get_fmt_list( vertices->get_raw( ) );

        for ( basic::uint32 i = 0; i < fmt_list.get_size( ); ++i )
        {
            const VertexFMT& fmt = fmt_list[ i ];
            glVertexAttribPointer( i,
                                   fmt.size,
                                   fmt.type,
                                   fmt.is_normalized,
                                   sizeof( T ),
                                   (const void*)fmt.offset );

            glEnableVertexAttribArray( i );
        }

        glBindVertexArray( 0 );

        if ( parent )
        {
            parent->add_child( this );
        }
    }

    template < typename VertexType >
    void
    update_vertices( basic::Vector< VertexType >* vertices )
    {
        ASSERT( vertices != nullptr );
        ASSERT( vertex_object > 0 );

        glBindBuffer( GL_ARRAY_BUFFER, vertex_object );
        basic::uint32 size = sizeof( VertexType ) * vertices->get_size( );
        basic::int32 need_elements = static_cast< basic::int32 >( vertices->get_size( ) );

        if ( vertex_elements == 0 || vertex_elements < need_elements )
        {
            glBufferData( GL_ARRAY_BUFFER, size, vertices->get_raw( ), get_buffer_usage( ) );

            vertex_elements = need_elements;

            return;
        }

        vertex_elements = need_elements;

        glBufferSubData( GL_ARRAY_BUFFER, 0, size, vertices->get_raw( ) );

        glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }

    void update_indices( IndexBuffer* indices );

    void draw_node( basic::uint32 prev_vao = 0 );

    static RenderNode* create_node( ShaderProgram* program, Texture* texture );

    static void remove_node( RenderNode* node );

    Transform* get_transform( ) const;

    void add_child( RenderNode* node );

    const Texture* get_texture( ) const;

    void set_order( basic::int32 order );

    friend struct RenderNodeOrderCmp;

private:
    basic::uint32 get_buffer_usage( ) const;

private:
    basic::uint32 array_object = 0;
    basic::uint32 vertex_object = 0;
    basic::int32 vertex_elements = 0;
    basic::uint32 index_object = 0;
    basic::int32 index_elements = 0;
    basic::uint32 flags = 0;
    basic::int32 order = 0;
    ICamera* camera = nullptr;
    Material* material = nullptr;
    Transform* transform = nullptr;
    basic::Color color;
    RenderNode* parent;
    basic::Vector< RenderNode* > children;
};

bool load_mesh( std::vector< uint8_t > data, MeshData& out_mesh, MeshLoadSettings settings );

void fill_line( const glm::vec2& p0, const glm::vec2& p1, float width, VertexBufferP& out_vb );

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

    void generate( VertexBufferT& out_vb, int offset, const struct TextureRect* rect = nullptr );

    void generate( IndexBuffer& out_ib, int offset );

    static IndexBuffer::Item indices[ 6 ];

private:
    glm::vec3 m_size;
    glm::vec2 m_anchor;
    basic::Color m_color;
};
