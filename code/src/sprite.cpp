#include "sprite.hpp"

Sprite::Sprite()
    : m_object()
    , m_color{ 255, 255, 255, 255 }
    , m_texture()
    , m_size( 1.f, 1.f, 0.f )
    , m_anchor( 0.5f, 0.5f )
{
}

void Sprite::init( Texture* texture )
{
    VertexBuffer vb;

    QuadGenerator generator( m_size, m_anchor, m_color );
    generator.generate( vb, 0 );

    m_object.set_vertex_buffer( std::move( vb ) );

    IndexBuffer ib;
    generator.generate( ib, 0 );

    m_object.set_index_buffer( std::move( ib ) );

    m_object.init();

    m_object.set_texture( texture );
}

void Sprite::draw( ICamera* camera, IRender* render ) 
{
    m_object.draw( render, camera );
}

void Sprite::set_position( const glm::vec3& pos )
{
    m_object.get_transform()->set_position( pos );        
}

glm::vec3 Sprite::get_position() const
{
    return m_object.get_transform()->get_position();
}

void Sprite::set_color( basic::uint8 r,
                basic::uint8 g,
                basic::uint8 b,
                basic::uint8 a )
{
    m_color = { r, g, b, a };

    if( m_object.is_initialized() )
    {
        m_object.update( &Sprite::update_color, this );
    }
}

void Sprite::set_size( float width, float height )
{
    m_size = { width, height, m_size.z };

    VertexBuffer vb;
    
    QuadGenerator generator( m_size, m_anchor, m_color );
    generator.generate( vb, 0 );

    m_object.set_vertex_buffer( std::move( vb ) );
    if( m_object.is_initialized() )
    {
        m_object.update( nullptr, nullptr );
    }
}

void Sprite::set_anchor( float x, float y )
{
    m_anchor = { x, y };

    VertexBuffer vb;
    QuadGenerator generator( m_size, m_anchor, m_color );
    generator.generate( vb, 0 );

    m_object.set_vertex_buffer( std::move( vb ) );

    if( m_object.is_initialized() )
    {
        m_object.update( nullptr, nullptr );
    }
}

void Sprite::update_color( Vertex* v, void* user_data )
{
    Sprite* sp = static_cast<Sprite*>( user_data );
    v->color = sp->m_color;
}

void Sprite::update_size( Vertex* v, void* user_data )
{
    Sprite* sp = static_cast<Sprite*>( user_data );
}


