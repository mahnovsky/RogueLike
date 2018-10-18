#include "sprite.hpp"

Sprite::Sprite()
    :m_object()
    ,m_color()
    ,m_texture()
{
    m_color = { 255, 255, 255, 255 };
}

void Sprite::init( const char* texture_file )
{
    VertexBuffer vb;
    vb.push( { { 0.5f, 0.5f, 0.0f }, m_color, { 1.f, 0.f } } );
    vb.push( { { 0.5f, -0.5f, 0.0f }, m_color, { 1.f, 1.f } } );
    vb.push( { {-0.5f, -0.5f, 0.0f }, m_color, { 1.f, 0.f } } );
    vb.push( { {-0.5f, 0.5f, 0.0f }, m_color, { 0.f, 0.f } } );

    m_object.set_vertex_buffer( std::move( vb ) );

    IndexBuffer ib;
    ib.push( 0 );
    ib.push( 1 );
    ib.push( 3 );
    ib.push( 1 );
    ib.push( 2 );
    ib.push( 3 );

    m_object.set_index_buffer( std::move( ib ) );

    m_object.init();

    basic::Vector<char> bmp_data = basic::get_file_content("my.bmp");

    basic::Image image;

    if( !bmp_data.is_empty() && basic::load_image( bmp_data, image ) )
    {
        LOG( "Image loaded successfuly w: %d, h: %d", image.width, image.height ); 
        m_texture.init( std::move( image ) );

        m_object.set_texture( &m_texture );
    }
    else
    {
        LOG( "Failed load bmp image" );
    }
}

void Sprite::draw( IRender* render ) 
{
    render->draw( &m_object );
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

void Sprite::update_color( Vertex* v, void* user_data )
{
    Sprite* sp = static_cast<Sprite*>( user_data );
    v->color = sp->m_color;
}


