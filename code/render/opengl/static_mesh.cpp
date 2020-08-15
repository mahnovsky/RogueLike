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
{
}

bool StaticMesh::load(ResourceStorage* rs)
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
		m_vbo.init<Vertex>(
			ogl::BufferType::Array, 
			ogl::BufferUsage::Static, 
			mesh_data.vb.data(), 
			mesh_data.vb.size());

		m_vertex_count = mesh_data.vb.size();

		if (!mesh_data.ib.empty())
		{
			m_ibo.init<uint16_t>(
				ogl::BufferType::Element,
				ogl::BufferUsage::Static,
				mesh_data.ib.data(),
				mesh_data.ib.size());

			m_index_count = mesh_data.ib.size();
		}

		Vertex v;
		m_fmt_list = ::get_fmt_list(&v);

		scene->destroy();

		return true;
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

const basic::Vector<VertexFMT>& StaticMesh::get_fmt_list() const
{
	return m_fmt_list;
}
