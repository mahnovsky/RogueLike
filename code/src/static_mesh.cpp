#include "static_mesh.hpp"

#include "render_common.hpp"

StaticMesh::StaticMesh( ObjectManager* obj_mng, const char* name )
    : FileResource( obj_mng, SharedObjectType::StaticMesh, name )
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

StaticMesh* StaticMesh::create(ObjectManager* obj_mng, const char* name, const MeshLoadSettings& settings)
{
	ASSERT(name);

	MeshData mesh_data;

	auto data = basic::get_file_content(name);
	if (!data.is_empty() && 
		load_mesh(std::move(data), mesh_data, settings))
	{
		auto static_mesh = NEW_OBJ(StaticMesh, obj_mng, name);

		static_mesh->m_vbo_u = create_buffer(GL_ARRAY_BUFFER,
			GL_STATIC_DRAW,
			mesh_data.vb.get_raw(),
			mesh_data.vb.get_size() * sizeof(Vertex));

		static_mesh->m_vertex_count = mesh_data.vb.get_size();

		if (!mesh_data.ib.is_empty())
		{
			static_mesh->m_vib_u = create_buffer(GL_ELEMENT_ARRAY_BUFFER,
				GL_STATIC_DRAW,
				mesh_data.ib.get_raw(),
				mesh_data.ib.get_size() * sizeof(basic::uint16));

			static_mesh->m_index_count = mesh_data.ib.get_size();
		}

		Vertex v;
		static_mesh->m_fmt_list = ::get_fmt_list(&v);

		return static_mesh;
	}

	return nullptr;
}
