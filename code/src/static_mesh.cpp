#include "static_mesh.hpp"

#include "render_common.hpp"
#include "OpenFBX/ofbx.h"
#include "fbx_helper.hpp"

StaticMesh::StaticMesh( GenericObjectManager* obj_mng, const char* name )
    : FileResource( obj_mng, name )
	, m_vbo_u(0)
	, m_vib_u(0)
	, m_vertex_count(0)
	, m_index_count(0)
{
}

bool StaticMesh::load(ResourceStorage* rs)
{
	return true;
}


basic::uint32
StaticMesh::get_vertex_count( ) const
{
    return m_vertex_count;
}

basic::uint32
StaticMesh::get_index_count( ) const
{
    return m_index_count;
}

basic::uint32 StaticMesh::get_vbo() const
{
	return m_vbo_u;
}

basic::uint32 StaticMesh::get_ibo() const
{
	return m_vib_u;
}

const basic::Vector<VertexFMT>& StaticMesh::get_fmt_list() const
{
	return m_fmt_list;
}

StaticMesh* StaticMesh::create(GenericObjectManager* obj_mng, const char* name, const MeshLoadSettings& settings)
{
	ASSERT(name);

	MeshData mesh_data;
	auto data = basic::get_file_content(name);

	ofbx::IScene* scene = nullptr; 
	
	if (!data.empty() && 
		(scene = ofbx::load(data.data(), data.size(), 0)) &&
		load_fbx_mesh(scene, 0, mesh_data))
	{
		auto static_mesh = NEW_OBJ(StaticMesh, obj_mng, name);

		static_mesh->m_vbo_u = create_buffer(GL_ARRAY_BUFFER,
			GL_STATIC_DRAW,
			mesh_data.vb.data(),
			mesh_data.vb.size() * sizeof(Vertex));

		static_mesh->m_vertex_count = mesh_data.vb.size();

		if (!mesh_data.ib.empty())
		{
			static_mesh->m_vib_u = create_buffer(GL_ELEMENT_ARRAY_BUFFER,
				GL_STATIC_DRAW,
				mesh_data.ib.data(),
				mesh_data.ib.size() * sizeof(basic::uint16));

			static_mesh->m_index_count = mesh_data.ib.size();
		}

		Vertex v;
		static_mesh->m_fmt_list = ::get_fmt_list(&v);

		return static_mesh;
	}

	return nullptr;
}
