#include "render_object.hpp"
#include "camera.hpp"
#include "render.hpp"
#include "render_common.hpp"
#include "shader.hpp"
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
QuadGenerator::generate( VertexBuffer& out_vb, int offset )
{
    float xoff = offset * m_size.x;
    float left = ( 0.f - m_anchor.x ) * m_size.x + xoff;
    float right = ( 1.f - m_anchor.x ) * m_size.x + xoff;
    float bottom = ( 0.f - m_anchor.y ) * m_size.y;
    float top = ( 1.f - m_anchor.y ) * m_size.y;
    float z = m_size.z;

    out_vb.push( {{right, top, z}, m_color, {1.f, 0.f}} );
    out_vb.push( {{right, bottom, z}, m_color, {1.f, 1.f}} );
    out_vb.push( {{left, bottom, z}, m_color, {1.f, 0.f}} );
    out_vb.push( {{left, top, z}, m_color, {0.f, 0.f}} );
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

RenderObject::RenderObject( )
    : m_vb( )
    , m_ib( )
    , m_array_object( 0 )
    , m_vertex_object( 0 )
    , m_index_object( 0 )
    , m_transform( )
    , m_texture( nullptr )
    , m_shader( nullptr )
{
    m_transform.allocate( );
}

RenderObject::~RenderObject( )
{
    m_transform.free( );
}

void
RenderObject::init( )
{
    ASSERT( !m_vb.is_empty( ) );
    // ASSERT( !m_ib.is_empty() );

    glGenVertexArrays( 1, &m_array_object );
    bind_array_object( true );

    init_vertex_buffer( );
    if ( !m_ib.is_empty( ) )
    {
        init_index_buffer( );
    }
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

void
RenderObject::update( vertex_update callback, void* user_data )
{
    if ( callback )
    {
        for ( size_t i = 0; i < m_vb.get_size( ); ++i )
        {
            callback( &m_vb[ i ], user_data );
        }
    }
    glBufferData(
            GL_ARRAY_BUFFER, sizeof( Vertex ) * m_vb.get_size( ), m_vb.get_raw( ), GL_STATIC_DRAW );
}

bool
RenderObject::is_initialized( ) const
{
    return m_array_object > 0 && m_vertex_object > 0;
}

void
RenderObject::bind_array_object( bool on ) const
{
    basic::uint32 object = ( on ? m_array_object : 0 );

    glBindVertexArray( object );
}

void
RenderObject::set_vertex_buffer( VertexBuffer buffer )
{
    m_vb = std::move( buffer );
}

void
RenderObject::set_index_buffer( IndexBuffer buffer )
{
    m_ib = std::move( buffer );
}

void
RenderObject::init_vertex_buffer( )
{
    glGenBuffers( 1, &m_vertex_object );

    glBindBuffer( GL_ARRAY_BUFFER, m_vertex_object );

    glBufferData(
            GL_ARRAY_BUFFER, sizeof( Vertex ) * m_vb.get_size( ), m_vb.get_raw( ), GL_STATIC_DRAW );
}

void
RenderObject::init_index_buffer( )
{
    glGenBuffers( 1, &m_index_object );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_index_object );

    const size_t size = sizeof( basic::uint16 ) * m_ib.get_size( );

    glBufferData( GL_ELEMENT_ARRAY_BUFFER, size, m_ib.get_raw( ), GL_STATIC_DRAW );
}

void
RenderObject::bind( ) const
{
    ASSERT( is_initialized( ) );

    bind_array_object( true );

    m_texture->bind( );
}

void
RenderObject::unbind( ) const
{
    m_texture->unbind( );

    bind_array_object( false );
}

void
RenderObject::draw( IRender* render, ICamera* cam ) const
{
    if ( !m_shader )
    {
        return;
    }

    m_shader->bind( );

    bind_array_object( true );

    glm::mat4 model = m_transform->get_matrix( );

    glm::mat4 pv( 1.f );
    cam->get_matrix( pv );

    render->push_mvp( pv * model );

    if ( m_texture )
    {
        m_texture->bind( );
    }
    if ( m_ib.is_empty( ) )
    {
        glDrawArrays( GL_TRIANGLES, 0, m_vb.get_size( ) );
    }
    else
    {
        glDrawElements( GL_TRIANGLES, get_element_count( ), GL_UNSIGNED_SHORT, NULL );
    }

	if (m_texture)
	{
		m_texture->unbind();
	}

    render->pop_mvp( );

    bind_array_object( false );

    m_shader->unbind( );
}

void
RenderObject::get_matrix( glm::mat4& out ) const
{
    out = m_transform->get_matrix( );
}

Transform*
RenderObject::get_transform( )
{
    return m_transform.get( );
}

const Transform*
RenderObject::get_transform( ) const
{
    return m_transform.get( );
}

void
RenderObject::set_texture( Texture* texture )
{
    m_texture = texture;
}

void
RenderObject::set_shader( Shader* shader )
{
    m_shader = shader;
}

size_t
RenderObject::get_element_count( ) const
{
    return m_ib.get_size( );
}
