#pragma once

#include "resource_storage.hpp"
#include "render_common.hpp"

#include "glbuffer.hpp"

class StaticMesh : public FileResource
{
public:
    StaticMesh( const char* name );
    ~StaticMesh( ) override = default;

	bool init(const MeshData& data, ogl::BufferUsage usage);

	bool load(ResourceStorage* rs);

    uint32_t get_vertex_count( ) const;

	uint32_t get_index_count( ) const;

	uint32_t get_vbo() const;

	uint32_t get_ibo() const;

	const basic::Vector<VertexFMT>& get_fmt_list() const;

private:

	ogl::Buffer m_vbo;
	ogl::Buffer m_ibo;

	basic::uint32 m_vertex_count;
	basic::uint32 m_index_count;

	basic::Vector<VertexFMT> m_fmt_list;
};
