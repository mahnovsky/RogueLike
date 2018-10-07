#include "render_object.hpp"

extern "C"
{
#define GLEW_STATIC
#include <GL/glew.h>
}

#include <GL/gl.h>


RenderObject::RenderObject()
    : m_vb()
    , m_ib()
    , m_array_object( 0 )
    , m_vertex_object( 0 )
    , m_index_object( 0 )
{
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

    size_t offset = sizeof( float ) * 3;
    glVertexAttribPointer( 1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof( Vertex ), (GLvoid*)offset );
    glEnableVertexAttribArray( 1 );


    bind_array_object( false );
}

void RenderObject::update( vertex_update callback, void* user_data )
{
    ASSERT( callback != nullptr );

    for( size_t i = 0; i < m_vb.get_size(); ++i )
    {
        callback( &m_vb[i], user_data );
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
}

void RenderObject::unbind() const
{
    bind_array_object( false );
}


