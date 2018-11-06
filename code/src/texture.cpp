#include "texture.hpp"
#include "GL/glew.h"
#include "basic/file.hpp"

Texture::Texture( )
    : m_texture( 0 )
{
}

Texture::Texture( Texture&& t )
    : m_texture( t.m_texture )
{
    t.m_texture = 0;
}

Texture::Texture( basic::uint32 w, basic::uint32 h, basic::uint32 tex )
    : m_width( w )
    , m_height( h )
    , m_texture( tex )
{
}

Texture::~Texture( )
{
    if ( m_texture )
    {
        glDeleteTextures( 1, &m_texture );
    }
}

Texture&
Texture::operator=( Texture&& t )
{
    m_texture = t.m_texture;
    m_width = t.m_width;
    m_height = t.m_height;
    m_components = t.m_components;

    t.m_texture = 0;
    return *this;
}

void
Texture::init( basic::Image image )
{
    init( image.width, image.height, std::move( image.data ), image.components );
}

void
Texture::init( basic::uint32 width,
               basic::uint32 height,
               basic::Vector< char > image_data,
               basic::ColorComponents cc )
{
    m_width = width;
    m_height = height;
    m_components = cc;

    glGenTextures( 1, &m_texture );

    glBindTexture( GL_TEXTURE_2D, m_texture );

    glTexImage2D( GL_TEXTURE_2D,
                  0,
                  GL_RGB,
                  width,
                  height,
                  0,
                  GL_BGR,
                  GL_UNSIGNED_BYTE,
                  image_data.get_raw( ) );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glGenerateMipmap( GL_TEXTURE_2D );
}

void
Texture::bind( ) const
{
    glActiveTexture( GL_TEXTURE0 );

    glBindTexture( GL_TEXTURE_2D, m_texture );
}

void
Texture::unbind( ) const
{
    glBindTexture( GL_TEXTURE_2D, 0 );
}

basic::uint32
Texture::get_width( ) const
{
    return m_width;
}

basic::uint32
Texture::get_height( ) const
{
    return m_height;
}

bool
load_texture( TextureCache& cache, const char* file, TextureCache::Handle& out_handle )
{
    basic::Vector< char > bmp_data = basic::get_file_content( file );

    basic::Image image;

    if ( !bmp_data.is_empty( ) && basic::load_image( bmp_data, image ) )
    {
        LOG( "Image loaded successfuly w: %d, h: %d", image.width, image.height );

        Texture* texture = new Texture( );

        texture->init( std::move( image ) );

        cache.add( texture, file, &out_handle );
    }
    else
    {
        LOG( "Failed load bmp image" );

        return false;
    }

    return true;
}
