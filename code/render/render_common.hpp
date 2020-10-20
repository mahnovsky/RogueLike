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

enum class VertexDataType
{
	Float,
	UnsignedByte,
	UnsignedShort,
	UnsignedInt
};

enum RenderStateFlags
{
	RSF_BLEND = 0x1,
	RSF_DEPTH_TEST = 0x2,
	RSF_CULL_TEST = 0x4
};

enum VertexFormat
{
	VF_XYZ = 0x1,
	VF_UV = 0x2,
	VF_NORMAL = 0x4,
	VF_COLOR_RGBA = 0x8
};

void deleter(void*);

struct VertexData
{
	uint32_t format = 0;
	uint32_t count = 0;
	uint32_t item_size = 0;
	std::unique_ptr<void, void(*)(void*)> data = {nullptr, deleter};
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

using VertexBufferP = std::vector< glm::vec3 >;
using VertexBufferT = std::vector< Vertex_T >;
using VertexBuffer = std::vector< Vertex >;
using IndexBuffer = std::vector< basic::uint16 >;

struct MeshData
{
	VertexData vertex_data;
    //VertexBuffer vertices;
    IndexBuffer indices;
};

void setup_vertices(VertexData& out, const VertexBuffer& vertices);

void setup_vertices(VertexData& out, const VertexBufferT& vertices);

std::vector<VertexFMT> get_vertex_format_description(uint32_t format_flags);

std::vector< VertexFMT > get_fmt_list( const glm::vec3* );

std::vector< VertexFMT > get_fmt_list( const Vertex* );

std::vector< VertexFMT > get_fmt_list( const Vertex_T* );

bool load_mesh( std::vector< uint8_t > data, MeshData& out_mesh );

class DrawingRect
{
public:
	DrawingRect(IRender* render);
	~DrawingRect();

	void set_position(const glm::vec2& pos);

	void set_size(const glm::vec2& size);

	void set_view_projection_matrix(const glm::mat4& vp);

	void draw();

	void update_rect();

	void init_rect();

private:
	IRender* m_render;
	IRenderObject* m_rect_object;
	VertexBuffer m_vertices;
	glm::vec2 m_pos;
	glm::vec2 m_size;
	glm::mat4 m_view_projection;
	bool m_need_update;
};
