#include "render_object.hpp"

extern "C"
{
#define GLEW_STATIC
#include <GL/glew.h>
}

#include <GL/gl.h>

#include "texture.hpp"

IndexBuffer::Item Quad::indices[6] = { 0, 1, 3, 1, 2, 3 }; 

void Quad::generate( VertexBuffer& out_vb, const glm::vec3& size, const glm::vec2& anchor, basic::Color c )
{
    float left = (0.f - anchor.x) * size.x;
    float right = (1.f - anchor.x) * size.x;
    float bottom = (0.f - anchor.y) * size.y;
    float top = (1.f - anchor.y) * size.y;
    float z = size.z;

    out_vb.push( { { right, top, z }, c, { 1.f, 0.f } } );
    out_vb.push( { { right, bottom, z }, c, { 1.f, 1.f } } );
    out_vb.push( { { left, bottom, z }, c, { 1.f, 0.f } } );
    out_vb.push( { { left, top, z }, c, { 0.f, 0.f } } );
}

RenderObject::RenderObject()
    : m_vb()
    , m_ib()
    , m_array_object( 0 )
    , m_vertex_object( 0 )
    , m_index_object( 0 )
    , m_transform()
    , m_texture( nullptr )
{
    m_transform.allocate();
}

RenderObject::~RenderObject()
{
    m_transform.free();
}

void RenderObject::init()
{
    ASSERT( !m_vb.is_empty() );
    ASSERT( !m_ib.is_empty() );

    glGenVertexArrays( 1, &m_array_object );
    bind_array_object( true );

    init_vertex_buffer();
    init_index_buffer();

    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (GLvoid*)0 );
    glEnableVertexAttribArray( 0 );

    size_t offset = sizeof( glm::vec3 );
    glVertexAttribPointer( 1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof( Vertex ), (GLvoid*)offset );
    glEnableVertexAttribArray( 1 );

    offset = sizeof( glm::vec3 ) + sizeof( basic::Color );
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ), (GLvoid*)offset );
    glEnableVertexAttribArray( 2 );

    bind_array_object( false );
}

void RenderObject::update( vertex_update callback, void* user_data )
{
    if( callback )
    {
        for( size_t i = 0; i < m_vb.get_size(); ++i )
        {
            callback( &m_vb[i], user_data );
        }
    }
    glBufferData( GL_ARRAY_BUFFER, sizeof( Vertex ) * m_vb.get_size(), m_vb.get_raw(), GL_STATIC_DRAW );
}

bool RenderObject::is_initialized() const
{
    return m_array_object > 0 && m_vertex_object > 0 && m_index_object > 0;
}

void RenderObject::bind_array_object( bool on ) const
{
    basic::uint32 object = ( on ? m_array_object : 0 );

    glBindVertexArray( object );
}

void RenderObject::set_vertex_buffer( VertexBuffer buffer )
{
    m_vb = std::move( buffer );
}

void RenderObject::set_index_buffer( IndexBuffer buffer )
{
    m_ib = std::move( buffer );
}

void RenderObject::init_vertex_buffer( )
{
    glGenBuffers( 1, &m_vertex_object );

    glBindBuffer( GL_ARRAY_BUFFER, m_vertex_object );

    glBufferData( GL_ARRAY_BUFFER, sizeof( Vertex ) * m_vb.get_size(), m_vb.get_raw(), GL_STATIC_DRAW );
}

void RenderObject::init_index_buffer( )
{
    glGenBuffers( 1, &m_index_object );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_index_object );

    const size_t size = sizeof( basic::uint16 ) * m_ib.get_size();

    glBufferData( GL_ELEMENT_ARRAY_BUFFER, size, m_ib.get_raw(), GL_STATIC_DRAW );
}

void RenderObject::bind() const
{
    ASSERT( is_initialized() );

    bind_array_object( true );

    m_texture->bind();
}

void RenderObject::unbind() const
{
    m_texture->unbind();

    bind_array_object( false );
}

void RenderObject::get_matrix( glm::mat4& out ) const
{
    m_transform->get_matrix( out );
}

Transform* RenderObject::get_transform() 
{
    return m_transform.get();
}

const Transform* RenderObject::get_transform() const
{
    return m_transform.get();
}

void RenderObject::set_texture( Texture* texture )
{
    m_texture = texture;
}

size_t RenderObject::get_element_count() const 
{ 
    return m_ib.get_size(); 
}

