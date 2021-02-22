#pragma once

#include "resource_storage.hpp"
#include "render_common.hpp"

#include "glbuffer.hpp"

class StaticMesh : public core::FileResource
{
public:
    StaticMesh( const char* name );
    ~StaticMesh( ) override = default;

	bool init(const MeshData& data, ogl::BufferUsage usage);

	void update(const MeshData& data);

	bool load(core::ResourceStorage* rs);

    uint32_t get_vertex_count( ) const;

	uint32_t get_index_count( ) const;

	uint32_t get_vbo() const;

	uint32_t get_ibo() const;

	uint32_t get_element_size() const;

	const std::vector<VertexFMT>& get_fmt_list() const;

private:

	ogl::Buffer m_vbo;
	ogl::Buffer m_ibo;

	uint32_t m_vertex_count;
	uint32_t m_index_count;
	uint32_t m_element_size;

	std::vector<VertexFMT> m_fmt_list;
};
