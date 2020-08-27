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

enum class VertexDrawMode
{
	Point,
	Line,
	LineStrip,
	Triangle,
	TriangleFan,
	TriangleStrip
};

enum class VertexBufferUsage
{
	Static,
	Dynamic,
	Streaming
};

enum class RenderResourceType
{
	StaticMesh,
	Texture,
	ShaderProgram,
	Count
};

struct VertexFMT
{
    uint32_t offset;
	uint32_t size;
	uint32_t type;
    uint8_t is_normalized;
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
    VertexBuffer vertices;
    IndexBuffer indices;
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


basic::Vector< VertexFMT > get_fmt_list( const glm::vec3* );

basic::Vector< VertexFMT > get_fmt_list( const Vertex* );

basic::Vector< VertexFMT > get_fmt_list( const Vertex_T* );

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
