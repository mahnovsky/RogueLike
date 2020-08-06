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
		ASSERT(false); \
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
	glm::vec3 normal;
};

using VertexBufferP = basic::Vector< glm::vec3 >;
using VertexBufferT = basic::Vector< Vertex_T >;
using VertexBuffer = std::vector< Vertex >;
using IndexBuffer = std::vector< basic::uint16 >;

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

bool load_mesh( std::vector< uint8_t > data, MeshData& out_mesh, MeshLoadSettings settings );

void fill_line( const glm::vec2& p0, const glm::vec2& p1, float width, VertexBufferP& out_vb );

void fill_rect( const glm::vec2& left_top,
                const glm::vec2& right_bottom,
                float width,
                VertexBufferP& out_vb );


struct QuadGenerator
{
    QuadGenerator( const glm::vec3& size, const glm::vec2& anchor, const basic::Color& color );

    void generate( VertexBufferT& out_vb, int offset, const struct TextureRect* rect = nullptr );

    void generate( IndexBuffer& out_ib, int offset );

    static std::uint16_t indices[ 6 ];

private:
    glm::vec3 m_size;
    glm::vec2 m_anchor;
    basic::Color m_color;
};
