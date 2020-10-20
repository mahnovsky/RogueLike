#include "static_mesh.hpp"

#include "render_common.hpp"
#include "OpenFBX/ofbx.h"
#include "fbx_helper.hpp"

StaticMesh::StaticMesh( const char* name )
    : FileResource( name )
	, m_vbo()
	, m_ibo()
	, m_vertex_count(0)
	, m_index_count(0)
	, m_element_size(0)
{
}

bool StaticMesh::init(const MeshData& data, ogl::BufferUsage usage)
{
	if (data.vertex_data.data == nullptr)
	{
		return false;
	}

	m_fmt_list = get_vertex_format_description(data.vertex_data.format);
	m_element_size = data.vertex_data.item_size;

	bool result = m_vbo.init_raw(ogl::BufferType::Array, usage, 
		data.vertex_data.data.get(), 
		data.vertex_data.count, 
		data.vertex_data.item_size);
	if (!result)
	{
		LOG("Failed to init VBO");
		return false;
	}

	//m_vbo.init_raw(ogl::BufferType::Array, usage, data.vertex_data.data, data.vertex_data.count,)
	/*m_vbo.init<Vertex>(
		ogl::BufferType::Array,
		usage,
		data.vertices.data(),
		data.vertices.size());*/

	m_vertex_count = data.vertex_data.count;

	if (!data.indices.empty())
	{
		m_ibo.init<uint16_t>(
			ogl::BufferType::Element,
			ogl::BufferUsage::Static,
			data.indices.data(),
			data.indices.size());

		m_index_count = data.indices.size();
	}

	return true;
}

bool StaticMesh::load(core::ResourceStorage* rs)
{
	auto file_name = get_file_name();

	MeshData mesh_data;
	auto data = basic::get_file_content(file_name.data());

	ofbx::IScene* scene = nullptr;
	std::uint64_t fbx_load_flags = static_cast<std::uint64_t>(ofbx::LoadFlags::TRIANGULATE);
	if (!data.empty() &&
		(scene = ofbx::load(data.data(), data.size(), fbx_load_flags)) &&
		load_fbx_mesh(scene, 0, mesh_data))
	{
		bool result = init(mesh_data, ogl::BufferUsage::Static);

		scene->destroy();

		return result;
	}

	return false;
}

uint32_t StaticMesh::get_vertex_count( ) const
{
    return m_vertex_count;
}

uint32_t StaticMesh::get_index_count( ) const
{
    return m_index_count;
}

uint32_t StaticMesh::get_vbo() const
{
	return m_vbo.get_handle();
}

uint32_t StaticMesh::get_ibo() const
{
	return m_ibo.get_handle();
}

uint32_t StaticMesh::get_element_size() const
{
	return m_element_size;
}

const std::vector<VertexFMT>& StaticMesh::get_fmt_list() const
{
	return m_fmt_list;
}
