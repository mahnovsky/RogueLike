#include "static_mesh.hpp"

#include "render_common.hpp"
#include "OpenFBX/ofbx.h"
#include "fbx_helper.hpp"
#include <glm/glm.hpp>

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
			usage,
			data.indices.data(),
			data.indices.size());

		m_index_count = data.indices.size();
	}

	update_bounding_sphere(data);

	return true;
}

void StaticMesh::update(const MeshData& data)
{
	m_element_size = data.vertex_data.item_size;

	m_vbo.update(data.vertex_data.data.get(), data.vertex_data.count);

	m_vertex_count = data.vertex_data.count;

	if (!data.indices.empty())
	{
		m_ibo.update(data.indices.data(), data.indices.size());

		m_index_count = data.indices.size();
	}

	update_bounding_sphere(data);
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

float StaticMesh::get_bounding_sphere_radius() const
{
	return m_bounding_sphere_radius;
}

void StaticMesh::update_bounding_sphere(const MeshData& data)
{
	uint32_t step = data.vertex_data.item_size;
	void* ptr = data.vertex_data.data.get();
	for (uint32_t i = 0; i < data.vertex_data.count; ++i)
	{
		auto vec = static_cast<glm::vec3*>(ptr);
		ptr = static_cast<char*>(ptr) + step;
		if (vec->x < m_min_corner.x)
		{
			m_min_corner.x = vec->x;
		}
		if (vec->y < m_min_corner.y)
		{
			m_min_corner.y = vec->y;
		}
		if (vec->z < m_min_corner.z)
		{
			m_min_corner.z = vec->z;
		}

		if (vec->x > m_max_corner.x)
		{
			m_max_corner.x = vec->x;
		}
		if (vec->y > m_max_corner.y)
		{
			m_max_corner.y = vec->y;
		}
		if (vec->z > m_max_corner.z)
		{
			m_max_corner.z = vec->z;
		}
	}
	
	m_bounding_sphere_radius = glm::length( (m_max_corner - m_min_corner) / 2.f );
}
