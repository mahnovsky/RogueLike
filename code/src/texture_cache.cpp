#include "texture_cache.hpp"

TextureCache::TextureCache()
    :m_textures()
    ,m_handle_counter( 0 )
{
}

TextureCache::~TextureCache()
{
    for( size_t i = 0; i < m_textures.get_size(); ++i)
    {
        delete m_textures[i].texture;
        m_textures[i].texture = nullptr;
    }
}

bool TextureCache::load_texture( const char* file, TextureHandle& out_handle )
{
    basic::Vector<char> bmp_data = basic::get_file_content( file );

    basic::Image image;

    if( !bmp_data.is_empty() && basic::load_image( bmp_data, image ) )
    {
        LOG( "Image loaded successfuly w: %d, h: %d", image.width, image.height ); 

        TextureInfo info;
        info.name = file;
        info.handle = m_handle_counter++;
        info.texture = new Texture();
        out_handle = info.handle;

        info.texture->init( std::move( image ) );
        
        m_textures.push( info );  
    }
    else
    {
        LOG( "Failed load bmp image" );

        return false;
    }

    return true;
}

Texture* TextureCache::get_texture( TextureHandle handle ) const
{
    for( size_t i = 0; i < m_textures.get_size(); ++i )
    {
        if( m_textures[i].handle == handle )
        {
            return m_textures[i].texture;
        }
    } 
    return nullptr;
}

TextureHandle TextureCache::get_texture_handle( const char* name )
{
    basic::String sname = name;

    for( size_t i = 0; i < m_textures.get_size(); ++i )
    {
        if( m_textures[i].name == sname )
        {
            return m_textures[i].handle;
        }
    } 
    
    return INVALID_HANDLE;
}


