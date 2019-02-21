#include "render_common.hpp"

#include "resource_storage.hpp"
#include "shader.hpp"
#include "material.hpp"
#include "transform.hpp"
#include "camera.hpp"
#include "texture.hpp"

struct EnableArrayBuffer
{
    EnableArrayBuffer( basic::uint32 object )
        :m_object(object)
    {
        if( m_object > 0 )
        {
            glBindVertexArray( m_object );
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
    node->material->~Material();
    node->transform->~Transform();

    node->material = nullptr;
    node->transform = nullptr;
    basic::mem_free( node );
}


void draw_node( RenderNode *node )
{
    if( !node || !node->material )
    {
        return;
    }

    EnableArrayBuffer ao( node->array_object );

    node->material->enable();

    glm::mat4 mvp(1.f);

    if( node->transform )
    {
        mvp = node->transform->get_matrix();
    }

    if( node->camera )
    {
        glm::mat4 pv( 1.f );
        node->camera->get_matrix( pv );

        mvp = pv * mvp;
    }

    node->material->load_matrix( "MVP", mvp );

    if( node->index_object > 0 )
    {
        glDrawElements( GL_TRIANGLES, node->index_elements, GL_UNSIGNED_SHORT, nullptr );
    }
    else
    {
        glDrawArrays( GL_TRIANGLES, 0, node->vertex_elements );
    }

    node->material->disable();
}

void init_node(RenderNode *node, VertexBuffer *vertices, IndexBuffer *indices )
{
    ASSERT( node != nullptr );
    ASSERT( vertices != nullptr );

    node->color = basic::Color{255,255,255,255};

    glGenVertexArrays( 1, &node->array_object );
    glBindVertexArray( node->array_object );

    glGenBuffers( 1, &node->vertex_object );
    update_vertices( node, vertices );

    if( indices )
    {
        glGenBuffers( 1, &node->index_object );

        update_indices( node, indices );
    }

    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), nullptr );
    glEnableVertexAttribArray( 0 );

    size_t offset = sizeof( glm::vec3 );
    glVertexAttribPointer( 1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof( Vertex ), (GLvoid*)offset );
    glEnableVertexAttribArray( 1 );

    offset = sizeof( glm::vec3 ) + sizeof( basic::Color );
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (GLvoid*)offset );
    glEnableVertexAttribArray( 2 );

    glBindVertexArray( 0 );
}

void init_node(RenderNode *node, VertexBufferT *vertices, IndexBuffer *indices )
{
    ASSERT( node != nullptr );
    ASSERT( vertices != nullptr );

    node->color = basic::Color{255,255,255,255};

    glGenVertexArrays( 1, &node->array_object );
    glBindVertexArray( node->array_object );

    glGenBuffers( 1, &node->vertex_object );
    update_vertices( node, vertices );

    if( indices )
    {
        glGenBuffers( 1, &node->index_object );

        update_indices( node, indices );
    }

    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex_T ), nullptr );
    glEnableVertexAttribArray( 0 );

    basic::uint32 offset = sizeof( glm::vec3 );

    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex_T ), (GLvoid*)offset );
    glEnableVertexAttribArray( 1 );

    glBindVertexArray( 0 );
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
