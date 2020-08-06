#include "render_common.hpp"

#include "camera.hpp"
#include "resource_storage.hpp"
#include "opengl/resource_shader.hpp"
#include "texture.hpp"
#include "transform.hpp"

#include <iostream>

std::uint16_t QuadGenerator::indices[ 6 ] = {0, 1, 3, 1, 2, 3};

QuadGenerator::QuadGenerator( const glm::vec3& size,
                              const glm::vec2& anchor,
                              const basic::Color& c )
    : m_size( size )
    , m_anchor( anchor )
    , m_color{c}
{
}

void
QuadGenerator::generate( VertexBufferT& out_vb, int offset, const TextureRect* rect )
{
    float xoff = offset * m_size.x;
    float left = ( 0.f - m_anchor.x ) * m_size.x + xoff;
    float right = ( 1.f - m_anchor.x ) * m_size.x + xoff;
    float bottom = ( 0.f - m_anchor.y ) * m_size.y;
    float top = ( 1.f - m_anchor.y ) * m_size.y;
    float z = m_size.z;

    glm::vec2 t_rt{1.f, 0.f};
    glm::vec2 t_rb{1.f, 1.f};
    glm::vec2 t_lb{0.f, 1.f};
    glm::vec2 t_lt{0.f, 0.f};
    if ( rect )
    {
        t_rt = {rect->x + rect->w, rect->y};
        t_rb = {rect->x + rect->w, rect->y + rect->h};
        t_lb = {rect->x, rect->y + rect->h};
        t_lt = {rect->x, rect->y};
    }
    out_vb.push( {{right, top, z}, t_rt} );
    out_vb.push( {{right, bottom, z}, t_rb} );
    out_vb.push( {{left, bottom, z}, t_lb} );
    out_vb.push( {{left, top, z}, t_lt} );
}

void
QuadGenerator::generate( IndexBuffer& out_vb, int offset )
{
    int xoff = offset * 6;
    for ( int i = 0; i < 6; ++i )
    {
        auto index = indices[ i ] + xoff;
        out_vb.push_back( index );
    }
}

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

bool load_mesh( std::vector< uint8_t > data, MeshData& out_mesh, MeshLoadSettings settings)
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

    
    out_mesh.vb.reserve( vert_coords.get_size( ) );

	basic::Color color{ 255, 255, 255, 255 };
	basic::Vector<MeshIndex> indices;
	indices.reserve(triangles.get_size() * 3);
	out_mesh.ib.reserve(triangles.get_size() * 3);
	out_mesh.vb.reserve(triangles.get_size() * 3);

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
				index.index = out_mesh.vb.size();
				out_mesh.vb.push_back({});
				Vertex& vertex = out_mesh.vb.back();

				vertex.pos = vert_coords[index.vertex_index];
				vertex.color = color;

				if (!tex_coords.is_empty())
					vertex.uv = tex_coords[index.texture_index];

				out_mesh.ib.push_back(index.index);
				indices.push(index);
			}
			else
			{
				out_mesh.ib.push_back(indices[pos].index);
			}
		}
    }

    return true;
}

basic::uint32 create_buffer( basic::uint32 buffer_type,
               basic::uint32 buffer_usage,
               const void* data,
               basic::uint32 size )
{
    basic::uint32 buffer;

    CHECKED_CALL(glGenBuffers, 1, &buffer );
    CHECKED_CALL(glBindBuffer, buffer_type, buffer );

    CHECKED_CALL(glBufferData, buffer_type, size, data, buffer_usage );

    return buffer;
}

void
fill_line( const glm::vec2& p0, const glm::vec2& p1, float width, VertexBufferP& out_vb )
{
    glm::vec3 pos0 = glm::vec3( p0, 0 );
    glm::vec3 pos1 = glm::vec3( p1, 0 );

    glm::vec3 delta = pos1 - pos0;
    float sq_dist = glm::dot( delta, delta );
    ASSERT( sq_dist > 0.1f );
    ASSERT( width > 0.1f );

    glm::vec3 up{0.f, 0.f, 1.f};
    glm::vec3 dir = glm::normalize( glm::cross( delta, up ) );

    //{ 0, 1, 2, 3, 2, 1 }
    const float half_width = width / 2;
    out_vb.push( ( dir * half_width ) + pos0 );
    out_vb.push( ( dir * half_width ) + pos1 );
    out_vb.push( ( dir * -half_width ) + pos0 );
    out_vb.push( ( dir * -half_width ) + pos1 );
    out_vb.push( ( dir * -half_width ) + pos0 );
    out_vb.push( ( dir * half_width ) + pos1 );
}

void
fill_rect( const glm::vec2& left_top,
           const glm::vec2& right_bottom,
           float width,
           VertexBufferP& out_vb )
{
    glm::vec2 left_bottom( left_top.x, right_bottom.y );
    glm::vec2 right_top( right_bottom.x, left_top.y );

    fill_line( left_bottom, left_top, width, out_vb );
    fill_line( left_top, right_top, width, out_vb );
    fill_line( right_top, right_bottom, width, out_vb );
    fill_line( right_bottom, left_bottom, width, out_vb );
}

basic::Vector< VertexFMT >
get_fmt_list( const glm::vec3* )
{
    VertexFMT fmt;
    fmt.size = 3;
    fmt.type = GL_FLOAT;
    fmt.offset = 0;
    fmt.is_normalized = GL_FALSE;

    basic::Vector< VertexFMT > res;
    res.push( fmt );
    return res;
}

basic::Vector< VertexFMT >
get_fmt_list( const Vertex* )
{
    basic::Vector< VertexFMT > res;

    VertexFMT fmt;
    fmt.size = 3;
    fmt.type = GL_FLOAT;
    fmt.offset = 0;
    fmt.is_normalized = GL_FALSE;

    res.push( fmt );

    fmt.size = 4;
    fmt.type = GL_UNSIGNED_BYTE;
    fmt.offset = sizeof( glm::vec3 );
    fmt.is_normalized = GL_TRUE;

    res.push( fmt );

    fmt.size = 2;
    fmt.type = GL_FLOAT;
    fmt.offset = sizeof(glm::vec3) + sizeof(basic::Color);
    fmt.is_normalized = GL_FALSE;

    res.push( fmt );

    return res;
}

basic::Vector< VertexFMT >
get_fmt_list( const Vertex_T* )
{
    basic::Vector< VertexFMT > res;

    VertexFMT fmt;
    fmt.size = 3;
    fmt.type = GL_FLOAT;
    fmt.offset = 0;
    fmt.is_normalized = GL_FALSE;

    res.push( fmt );

    fmt.size = 2;
    fmt.type = GL_FLOAT;
    fmt.offset = sizeof( float ) * 3;
    fmt.is_normalized = GL_FALSE;

    res.push( fmt );

    return res;
}

basic::int32 get_uniform(basic::uint32 program, const char* name)
{
	return glGetUniformLocation(program, name);
}

void set_uniform(basic::uint32 program, const char* name, const glm::vec2& v)
{
	basic::int32 pos = get_uniform(program, name);

	glUniform2f(pos, v.x, v.y);
}

void set_uniform(basic::uint32 program, const char* name, basic::int32 v)
{
	basic::int32 pos = get_uniform(program, name);

	glUniform1i(pos, v);
}

void set_uniform(basic::uint32 program, const char* name, const basic::Color& color)
{
	basic::int32 pos = get_uniform(program, name);

	glm::vec4 c{ static_cast<float>(color.red) / 255,
		static_cast<float>(color.blue) / 255,
		static_cast<float>(color.green) / 255,
		static_cast<float>(color.alpha) / 255 };

	glUniform4fv(pos, 1, glm::value_ptr(c));
}

void set_uniform(basic::uint32 program, const char* name, const glm::mat4& mat)
{
	basic::int32 pos = get_uniform(program, name);

	glUniformMatrix4fv(pos, 1, GL_FALSE, glm::value_ptr(mat));
}