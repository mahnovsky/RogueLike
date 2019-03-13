#include "render_common.hpp"

#include "resource_storage.hpp"
#include "shader.hpp"
#include "material.hpp"
#include "transform.hpp"
#include "camera.hpp"
#include "texture.hpp"

IndexBuffer::Item QuadGenerator::indices[ 6 ] = {0, 1, 3, 1, 2, 3};

QuadGenerator::QuadGenerator( const glm::vec3& size,
                              const glm::vec2& anchor,
                              const basic::Color& c )
    : m_size( size )
    , m_anchor( anchor )
    , m_color{c}
{
}

void
QuadGenerator::generate( VertexBufferT& out_vb, int offset )
{
    float xoff = offset * m_size.x;
    float left = ( 0.f - m_anchor.x ) * m_size.x + xoff;
    float right = ( 1.f - m_anchor.x ) * m_size.x + xoff;
    float bottom = ( 0.f - m_anchor.y ) * m_size.y;
    float top = ( 1.f - m_anchor.y ) * m_size.y;
    float z = m_size.z;

    out_vb.push( {{right, top, z}, {1.f, 0.f}} );
    out_vb.push( {{right, bottom, z}, {1.f, 1.f}} );
    out_vb.push( {{left, bottom, z}, {0.f, 1.f}} );
    out_vb.push( {{left, top, z}, {0.f, 0.f}} );
}

void
QuadGenerator::generate( IndexBuffer& out_vb, int offset )
{
    int xoff = offset * 6;
    for ( int i = 0; i < 6; ++i )
    {
        auto index = indices[ i ] + xoff;
        out_vb.push( index );
    }
}

struct EnableArrayBuffer
{
    EnableArrayBuffer( basic::uint32 object, basic::uint32& prev_vao )
        :m_object(object)
    {
        if(prev_vao > 0 && prev_vao == m_object)
        {
            m_object = 0;
            return;
        }

        if( m_object > 0 )
        {
            glBindVertexArray( m_object );
            prev_vao = m_object;
        }
    }

    ~EnableArrayBuffer()
    {
        if( m_object > 0 )
        {
            glBindVertexArray( 0 );
        }
    }
private:
    basic::uint32 m_object;
};

static basic::uint16
convert( basic::String str )
{
    return static_cast< basic::uint16 >( atoi( str.get_cstr( ) ) );
}

struct MeshVertex
{
    glm::vec3 pos;
    glm::vec2 uv;
    basic::uint16 index;
    basic::uint16 vi;
    basic::uint16 ti;
};

bool
load_mesh( const char* file, Mesh& out_mesh )
{
	basic::Vector< basic::uint8 > data{basic::get_file_content( file )};

    if ( data.is_empty( ) )
    {
        return false;
    }

    basic::Vector< glm::vec3 > vert_coords;
	vert_coords.reserve(1000);
    basic::Vector< glm::vec2 > tex_coords;
    basic::Vector< MeshVertex > vertexes;
	vertexes.reserve(1000);

    basic::uint32 offset = 0;
	int line_counter = 0;

	double s = basic::get_milliseconds() ;
	
	basic::String line;

    while ( offset < data.get_size( ) )
    {
		++line_counter;
		
		offset += basic::String::read_line( reinterpret_cast<basic::char_t*>(data.get_raw( ) + offset),
                                            static_cast<basic::uint32>(data.get_size( ) - offset), line );

        if ( offset >= data.get_size( ) )
        {
            break;
        }

        if ( line.front( ) == '#' || line.is_empty( ) )
        {
            continue;
        }

        basic::Vector< basic::String > items;
        line.split( items, ' ' );

        if( items.is_empty() )
        {
            continue;
        }

        if ( items.front( ) == "v" && items.get_size( ) > 3 )
        {
            float x = static_cast<float>( atof( items[ 1 ].get_cstr( ) ) );
            float y = static_cast<float>( atof( items[ 2 ].get_cstr( ) ) );
            float z = static_cast<float>( atof( items[ 3 ].get_cstr( ) ) );

            vert_coords.push( {x, y, z} );
        }
        else if ( items.front( ) == "vt" && items.get_size( ) > 2 )
        {
            float tx = static_cast<float>( atof( items[ 1 ].get_cstr( ) ) );
            float ty = static_cast<float>( atof( items[ 2 ].get_cstr( ) ) );

            tex_coords.push( {tx, ty} );
        }
        else if ( items.front( ) == "f" && items.get_size( ) > 3 )
        {
            basic::Vector< basic::uint16 > faces;
            items[ 1 ].split_to< basic::uint16 >( faces, '/', convert );
            items[ 2 ].split_to< basic::uint16 >( faces, '/', convert );
            items[ 3 ].split_to< basic::uint16 >( faces, '/', convert );

            basic::uint32 step = faces.get_size( ) >= 6 ? 2 : 1;
            for ( basic::uint32 i = 0; i < faces.get_size( ); i += step )
            {
                MeshVertex v;
                v.vi = faces[ i ];
                v.pos = vert_coords.get( v.vi - 1 );

                if ( step > 1 && !tex_coords.is_empty() )
                {
                    v.ti = faces[ i + 1 ];
                    v.uv = tex_coords.get( faces[ v.ti - 1 ] );
                }

                int index = -1;
                bool has_vertex = false;
                /*(for ( basic::uint32 i = 0; i < vertexes.get_size( ); ++i )
                {
                    auto& vv = vertexes[ i ];
                    has_vertex = vv.vi == v.vi && vv.ti == v.ti;
                    if ( has_vertex )
                    {
                        index = vv.index;
                        break;
                    }
                }*/
                if ( index < 0 )
                {
                    index = vertexes.get_size( );
                    v.index = index;
                    vertexes.push( v );
                }

                out_mesh.ib.push( index );
            }
        }
    }

	double delta = basic::get_milliseconds() - s;
	delta = 0.0;

    out_mesh.vb.reserve( vertexes.get_size( ) );
    for ( basic::uint32 i = 0; i < vertexes.get_size( ); ++i )
    {
        auto& vv = vertexes[ i ];

        out_mesh.vb.push( {vv.pos, {255, 255, 255, 255}, vv.uv} );
    }

    return true;
}

RenderNode *create_node( ShaderProgram* program, Texture* texture )
{
    RenderNode* node = nullptr;

    if( program )
    {
        void* objects_ptr = ALLOC_OBJECTS(RenderNode, Material, Transform);

        basic::uint32 offset = 0;
        node = basic::init_object<RenderNode>(objects_ptr, offset);
        node->material = basic::init_object<Material>(objects_ptr, offset, program, texture);
        node->transform = basic::init_object<Transform>(objects_ptr, offset);
    }

    return node;
}

void remove_node( RenderNode *node )
{
    if( !node )
    {
        return;
    }
    for(basic::uint32 i = 0; i < node->children.get_size(); ++i)
    {
        remove_node(node->children[i]);
        node->children[i] = nullptr;
    }

    node->material->~Material();
    node->transform->~Transform();

    if(node->vertex_object > 0)
    {
        glDeleteBuffers(1, &node->vertex_object);
    }
    if(node->index_object > 0)
    {
        glDeleteBuffers(1, &node->index_object);
    }
    if(node->array_object > 0)
    {
        glDeleteVertexArrays(1, &node->array_object);
    }

    node->material = nullptr;
    node->transform = nullptr;
    node->~RenderNode();
    basic::mem_free( node );
}


void draw_node(RenderNode *node, glm::mat4* mat, basic::uint32 prev_vao )
{
    if( !node || !node->material )
    {
        return;
    }

    if( prev_vao > 0 && (node->flags | USE_PARENT_VAO) == 0 )
    {
        prev_vao = 0;
    }

    EnableArrayBuffer ao( node->array_object, prev_vao );

    node->material->enable();

    glm::mat4 mvp(1.f);

    if( node->transform )
    {
        mvp = node->transform->get_matrix();
        if(mat)
        {
            mvp = mvp * (*mat);
        }
    }
    glm::mat4 model = mvp;
    if( node->camera )
    {
        glm::mat4 pv( 1.f );
        node->camera->get_matrix( pv );

        mvp = pv * mvp;
    }

    node->material->load_matrix( "MVP", mvp );
    node->material->load_color( "Color", node->color );

    if( node->index_object > 0 )
    {
        glDrawElements( GL_TRIANGLES, node->index_elements, GL_UNSIGNED_SHORT, nullptr );
    }
    else if( node->vertex_elements > 0 )
    {
        glDrawArrays( GL_TRIANGLES, 0, node->vertex_elements );
    }

    node->material->disable();

    for(basic::uint32 i = 0; i < node->children.get_size(); ++i)
    {
        draw_node(node->children[i], &mvp, node->array_object);
    }
}

basic::uint32 create_buffer( basic::uint32 buffer_type,
                             basic::uint32 buffer_usage,
                             void* data,
                             basic::uint32 size )
{
    basic::uint32 buffer;

    glGenBuffers( 1, &buffer );
    glBindBuffer( buffer_type, buffer );

    glBufferData( buffer_type,
                  size,
                  data,
                  buffer_usage );

    return buffer;
}

void update_indices( RenderNode *node, IndexBuffer *indices )
{
    ASSERT( node != nullptr );
    ASSERT( indices != nullptr );
    ASSERT( node->index_object > 0 );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, node->index_object );
    const basic::uint32 size = sizeof( basic::uint16 ) * indices->get_size( );

    glBufferData( GL_ELEMENT_ARRAY_BUFFER, size, indices->get_raw( ), GL_STATIC_DRAW );

    basic::int32 need_elements = static_cast<basic::int32>( indices->get_size() );

    if( node->index_elements == 0 || node->index_elements < need_elements )
    {
        glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                      size,
                      indices->get_raw( ),
                      GL_STATIC_DRAW );

        node->index_elements = need_elements;

        return;
    }

    node->index_elements = need_elements;

    glBufferSubData( GL_ELEMENT_ARRAY_BUFFER, 0, size, indices->get_raw() );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

void fill_line( const glm::vec2 &p0,
                const glm::vec2 &p1,
                float width,
                VertexBufferP& out_vb )
{
    glm::vec3 pos0 = glm::vec3(p0, 0);
    glm::vec3 pos1 = glm::vec3(p1, 0);

    glm::vec3 delta = pos1 - pos0;
    float sq_dist = glm::dot(delta, delta);
    ASSERT(sq_dist > 0.1f);
    ASSERT(width > 0.1f);

    glm::vec3 up{ 0.f, 0.f, 1.f };
    glm::vec3 dir = glm::normalize(glm::cross(delta, up));

    //{ 0, 1, 2, 3, 2, 1 }
    const float half_width = width / 2;
    out_vb.push( (dir * half_width) + pos0);
    out_vb.push( (dir * half_width) + pos1);
    out_vb.push( (dir * -half_width) + pos0);
    out_vb.push( (dir * -half_width) + pos1);
    out_vb.push( (dir * -half_width) + pos0);
    out_vb.push( (dir * half_width) + pos1);
}

void fill_rect( const glm::vec2 &left_top,
                const glm::vec2 &right_bottom,
                float width,
                VertexBufferP &out_vb)
{
    glm::vec2 left_bottom( left_top.x, right_bottom.y );
    glm::vec2 right_top( right_bottom.x, left_top.y );

    fill_line( left_bottom, left_top, width, out_vb );
    fill_line( left_top, right_top, width, out_vb );
    fill_line( right_top, right_bottom, width, out_vb );
    fill_line( right_bottom, left_bottom, width, out_vb );
}

RenderNode *make_rect( ShaderProgram *shader,
                       const glm::vec2 &left_top,
                       const glm::vec2 &right_bottom,
                       float width )
{
    RenderNode* res = create_node(shader, nullptr);

    VertexBufferP vb;
    fill_rect( left_top, right_bottom, width, vb );

    init_node( res, &vb, nullptr );

    return  res;
}

basic::uint32 get_buffer_usage(RenderNode *node)
{
    basic::uint32 buffer_usage = (node->flags & USE_DYNAMIC_VBO) == 0 ?
                GL_STATIC_DRAW : GL_DYNAMIC_DRAW;

    return buffer_usage;
}

basic::Vector<VertexFMT> get_fmt_list(glm::vec3 *)
{
    VertexFMT fmt;
    fmt.size = 3;
    fmt.type = GL_FLOAT;
    fmt.offset = 0;
    fmt.is_normalized = GL_FALSE;

    basic::Vector<VertexFMT> res;
    res.push(fmt);
    return res;
}

basic::Vector<VertexFMT> get_fmt_list(Vertex *)
{
    basic::Vector<VertexFMT> res;

    VertexFMT fmt;
    fmt.size = 3;
    fmt.type = GL_FLOAT;
    fmt.offset = 0;
    fmt.is_normalized = GL_FALSE;

    res.push(fmt);

    fmt.size = 4;
    fmt.type = GL_UNSIGNED_BYTE;
    fmt.offset = sizeof (float) * 3;
    fmt.is_normalized = GL_TRUE;

    res.push(fmt);

    fmt.size = 2;
    fmt.type = GL_FLOAT;
    fmt.offset = sizeof (float) * 3 + 4;
    fmt.is_normalized = GL_FALSE;

    res.push(fmt);

    return res;
}

basic::Vector<VertexFMT> get_fmt_list(Vertex_T *)
{
    basic::Vector<VertexFMT> res;

    VertexFMT fmt;
    fmt.size = 3;
    fmt.type = GL_FLOAT;
    fmt.offset = 0;
    fmt.is_normalized = GL_FALSE;

    res.push(fmt);

    fmt.size = 2;
    fmt.type = GL_FLOAT;
    fmt.offset = sizeof (float) * 3;
    fmt.is_normalized = GL_FALSE;

    res.push(fmt);

    return res;
}
