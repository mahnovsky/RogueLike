#include "render_common.hpp"

#include "camera.hpp"
#include "resource_storage.hpp"
#include "opengl/resource_shader.hpp"
#include "texture.hpp"
#include "transform.hpp"
#include "render.hpp"

#include <iostream>

struct EnableArrayBuffer
{
    EnableArrayBuffer( basic::uint32 object, basic::uint32& prev_vao )
        : m_object( object )
    {
        if ( prev_vao > 0 && prev_vao == m_object )
        {
            m_object = 0;
            return;
        }

        if ( m_object > 0 )
        {
            glBindVertexArray( m_object );
            prev_vao = m_object;
        }
    }

    ~EnableArrayBuffer( )
    {
        if ( m_object > 0 )
        {
            glBindVertexArray( 0 );
        }
    }

private:
    basic::uint32 m_object;
};

struct MeshIndex
{
	basic::uint16 index = 0;
    basic::uint16 vertex_index = 0;
    basic::uint16 texture_index = 0;
	basic::uint16 normal_index = 0;
};

bool operator == (const MeshIndex& a, const MeshIndex& b)
{
	return a.normal_index == b.normal_index &&
		a.vertex_index == b.vertex_index &&
		a.texture_index == b.texture_index;
}

bool load_mesh( std::vector< uint8_t > data, MeshData& out_mesh )
{
    basic::Vector< glm::vec3 > vert_coords;
    basic::Vector< glm::vec2 > tex_coords;
    
    vert_coords.reserve( 1000 );
    tex_coords.reserve( 1000 );
    
    basic::uint32 offset = 0;
    int line_counter = 0;

	struct Triangle
	{
		basic::uint16 vertex_indices[3] = { 0 };
		basic::uint16 texture_indices[3] = { 0 };
		basic::uint16 normal_indices[3] = { 0 };
	};

	basic::Vector<Triangle> triangles;
	triangles.reserve(1000);

    basic::String line;

    while ( offset < data.size( ) )
    {
        ++line_counter;

        if ( offset >= data.size( ) )
        {
            break;
        }

        offset += basic::String::read_line(
                reinterpret_cast< basic::char_t* >( data.data( ) + offset ),
            reinterpret_cast< basic::uint32 >( data.data( ) - offset ),
                line );

        if (line.is_empty() || line.front( ) == '#' )
        {
			++offset;
            continue;
        }

        basic::Vector< basic::String > items;
        line.split( items, ' ' );

        if ( items.is_empty( ) )
        {
            continue;
        }

        if ( items.front( ) == "v" && items.get_size( ) > 3 )
        {
			basic::conv::ConvStatus status;

			float x1 = basic::string_to<float>(items[1], status);
			ASSERT(status == basic::conv::ConvStatus::Ok);

			float y1 = basic::string_to<float>(items[2], status);
			ASSERT(status == basic::conv::ConvStatus::Ok);

			float z1 = basic::string_to<float>(items[3], status);
			ASSERT(status == basic::conv::ConvStatus::Ok);
			

            vert_coords.push( {x1, y1, z1} );
        }
        else if ( items.front( ) == "vt" && items.get_size( ) > 2 )
        {
			basic::conv::ConvStatus status;
			auto tx = basic::string_to<float>(items[1], status);
			auto ty = basic::string_to<float>(items[2], status);

            tex_coords.push( {tx, ty} );
        }
        else if ( items.front( ) == "f" && items.get_size( ) > 3 )
        {
            basic::Vector< basic::uint16 > faces;
            items[ 1 ].split_to< basic::uint16 >( faces, '/');
            items[ 2 ].split_to< basic::uint16 >( faces, '/');
            items[ 3 ].split_to< basic::uint16 >( faces, '/');

			triangles.push({});
			Triangle& triangle = triangles.back();

            const basic::uint32 step = faces.get_size( ) / 3;
			ASSERT(step > 0);
			ASSERT(step <= 3);

			basic::uint32 step_counter = 0;
			while (step_counter < step)
			{
				for (basic::uint32 i = 0; i < 3; ++i)
				{
					switch (step_counter)
					{
					case 0: triangle.vertex_indices[i] = faces[i * step] - 1; break;
					case 1: triangle.texture_indices[i] = faces[(i * step) + 1] - 1; break;
					case 2: triangle.normal_indices[i] = faces[(i * step) + 2] - 1; break;
					default: break;
					}
					
				}
				++step_counter;
			}
        }
    }

    
    //out_mesh.vertices.reserve( vert_coords.get_size( ) );

	basic::Color color{ 255, 255, 255, 255 };
	basic::Vector<MeshIndex> indices;
	indices.reserve(triangles.get_size() * 3);
	out_mesh.indices.reserve(triangles.get_size() * 3);
	//out_mesh.vertices.reserve(triangles.get_size() * 3);

    for ( auto& triangle : triangles )
    {
		for (int v = 0; v < 3; ++v)
		{
			MeshIndex index;
			index.vertex_index = triangle.vertex_indices[v];
			index.texture_index = triangle.texture_indices[v];
			index.normal_index = triangle.normal_indices[v];
			
			basic::uint32 pos = 0;
			if (!indices.find_first(pos, index))
			{
				//index.index = out_mesh.vertices.size();
				//out_mesh.vertices.push_back({});
				Vertex vertex;// = out_mesh.vertices.back();

				vertex.pos = vert_coords[index.vertex_index];
				vertex.color = color;

				if (!tex_coords.is_empty())
					vertex.uv = tex_coords[index.texture_index];

				out_mesh.indices.push_back(index.index);
				indices.push(index);
			}
			else
			{
				out_mesh.indices.push_back(indices[pos].index);
			}
		}
    }

    return true;
}

void deleter(void* dat)
{
	basic::mem_free(dat);
}

void setup_vertices(VertexData& out, const VertexBuffer& vertices)
{
	if (out.data != nullptr &&
		out.count >= vertices.size() &&
		out.item_size == sizeof(Vertex))
	{
		const size_t bytes = static_cast<size_t>(out.count) * out.item_size;
		memcpy(out.data.get(), vertices.data(), bytes);
	}
	else
	{
		out.format = VF_XYZ | VF_COLOR_RGBA | VF_UV | VF_NORMAL;
		out.count = vertices.size();
		out.item_size = sizeof(Vertex);
		out.data = { basic::mem_alloc(out.count * out.item_size), deleter };

		const size_t bytes = static_cast<size_t>(out.count) * out.item_size;
		memcpy(out.data.get(), vertices.data(), bytes);
	}
}

void setup_vertices(VertexData& out, const VertexBufferT& vertices)
{
	if (out.data != nullptr && 
		out.count >= vertices.size() &&
		out.item_size == sizeof(Vertex_T))
	{
		const size_t bytes = static_cast<size_t>(out.count) * out.item_size;
		memcpy(out.data.get(), vertices.data(), bytes);
	}
	else
	{
		out.format = VF_XYZ | VF_UV;
		out.count = vertices.size();
		out.item_size = sizeof(Vertex_T);
		out.data = { basic::mem_alloc(out.count * out.item_size), deleter };

		const size_t bytes = static_cast<size_t>(out.count) * out.item_size;
		memcpy(out.data.get(), vertices.data(), bytes);
	}
}

// order pos, color, uv, normals
std::vector<VertexFMT> get_vertex_format_description(uint32_t format_flags)
{
	std::vector<VertexFMT> result;
	VertexFMT fmt;
	int offset = 0;
	if ((format_flags & VF_XYZ) > 0)
	{
		fmt.is_normalized = 0;
		fmt.offset = 0;
		fmt.size = 3;
		fmt.type = GL_FLOAT;
		offset += sizeof(glm::vec3);

		result.push_back(fmt);
	}

	if ((format_flags & VF_COLOR_RGBA) > 0)
	{
		fmt.is_normalized = 1;
		fmt.offset = offset;
		fmt.size = 4;
		fmt.type = GL_UNSIGNED_BYTE;
		offset += sizeof(basic::Color);

		result.push_back(fmt);
	}

	if ((format_flags & VF_UV) > 0)
	{
		fmt.is_normalized = 0;
		fmt.offset = offset;
		fmt.size = 2;
		fmt.type = GL_FLOAT;
		offset += sizeof(glm::vec2);

		result.push_back(fmt);
	}

	if ((format_flags & VF_NORMAL) > 0)
	{
		fmt.is_normalized = 0;
		fmt.offset = offset;
		fmt.size = 3;
		fmt.type = GL_FLOAT;
		offset += sizeof(glm::vec3);

		result.push_back(fmt);
	}

	return result;
}

std::vector< VertexFMT > get_fmt_list( const glm::vec3* )
{
    VertexFMT fmt;
    fmt.size = 3;
    fmt.type = GL_FLOAT;
    fmt.offset = 0;
    fmt.is_normalized = GL_FALSE;

    std::vector< VertexFMT > res;
    res.push_back( fmt );

    return res;
}

std::vector< VertexFMT > get_fmt_list( const Vertex* )
{
    std::vector< VertexFMT > res;

    VertexFMT fmt;
    fmt.size = 3;
    fmt.type = GL_FLOAT;
    fmt.offset = 0;
    fmt.is_normalized = GL_FALSE;

    res.push_back( fmt );

    fmt.size = 4;
    fmt.type = GL_UNSIGNED_BYTE;
    fmt.offset = sizeof( glm::vec3 );
    fmt.is_normalized = GL_TRUE;

    res.push_back( fmt );

    fmt.size = 2;
    fmt.type = GL_FLOAT;
    fmt.offset = sizeof(glm::vec3) + sizeof(basic::Color);
    fmt.is_normalized = GL_FALSE;

    res.push_back( fmt );

    return res;
}

std::vector< VertexFMT > get_fmt_list( const Vertex_T* )
{
	std::vector< VertexFMT > res;

    VertexFMT fmt;
    fmt.size = 3;
    fmt.type = GL_FLOAT;
    fmt.offset = 0;
    fmt.is_normalized = GL_FALSE;

    res.push_back( fmt );

    fmt.size = 2;
    fmt.type = GL_FLOAT;
    fmt.offset = sizeof( float ) * 3;
    fmt.is_normalized = GL_FALSE;

    res.push_back( fmt );

    return res;
}

DrawingRect::DrawingRect(IRender* render)
	:m_render(render)
	,m_rect_object(nullptr)
	,m_vertices()
	,m_pos()
	,m_size(1.f, 1.f)
{
	init_rect();
}

DrawingRect::~DrawingRect()
{
	m_render->delete_object(m_rect_object);
}

void DrawingRect::set_position(const glm::vec2& pos)
{
	m_pos = pos;
	m_need_update = true;
}

void DrawingRect::set_size(const glm::vec2& size)
{
	m_size = size;
	m_need_update = true;
}

void DrawingRect::set_view_projection_matrix(const glm::mat4& vp)
{
	m_view_projection = vp;
	m_need_update = true;
}

void DrawingRect::draw()
{
	if (m_need_update)
	{
		update_rect();
	}

	m_render->add_to_frame(m_rect_object);
}

void DrawingRect::update_rect()
{
	auto& data = m_rect_object->get_mesh_data();
	glm::vec2 left_bottom = m_pos;
	glm::vec2 right_top = m_pos + m_size;

	m_vertices.clear();
	Vertex pos;
	pos.color = { 255, 255, 255, 255 };
	pos.pos = { left_bottom.x, left_bottom.y, 0.f }; // left-bottom
	m_vertices.push_back(pos);

	pos.pos = { left_bottom.x, right_top.y, 0.f }; // left-top
	m_vertices.push_back(pos);

	pos.pos = { right_top.x, right_top.y, 0.f }; // right-top
	m_vertices.push_back(pos);

	pos.pos = { right_top.x, left_bottom.y, 0.f }; // right-bottom
	m_vertices.push_back(pos);

	pos.pos = { left_bottom.x, left_bottom.y, 0.f }; // left-bottom
	m_vertices.push_back(pos);

	setup_vertices(data.vertex_data, m_vertices);
	
	m_rect_object->update_mvp(m_view_projection);

	m_rect_object->update_mesh_data();

	m_need_update = false;
}

void DrawingRect::init_rect()
{
	if (!m_rect_object)
	{
		m_rect_object = m_render->create_object();
	}
	m_rect_object->set_vertex_buffer_usage(VertexBufferUsage::Dynamic);
	m_rect_object->set_vertex_draw_mode(VertexDrawMode::LineStrip);
	m_rect_object->set_resource(RenderResourceType::ShaderProgram, "default");
	m_rect_object->update_color({ 0, 255, 0, 255 });

	update_rect();
}
