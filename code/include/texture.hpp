#pragma once

#include "basic/image.hpp"
#include "basic/string.hpp"
#include "cache.hpp"

class Texture
{
public:
    Texture( ) noexcept;
    Texture( Texture&& t ) noexcept;
    Texture( basic::uint32 w, basic::uint32 h, basic::uint32 tex ) noexcept;
    ~Texture( );

    Texture& operator=( Texture&& t );

    void init( basic::Image image );

    void init(basic::uint32 width,
               basic::uint32 height,
               basic::Vector<basic::uint8> image_data,
               basic::uint32 cc );

    void bind( ) const;

    void unbind( ) const;

    basic::uint32 get_width( ) const;

    basic::uint32 get_height( ) const;

private:
    basic::uint32 m_texture;
    basic::uint32 m_width;
    basic::uint32 m_height;
    basic::uint32 m_components;
};

using TextureCache = Cache< Texture >;

bool load_texture( TextureCache& cache, const char* file, TextureCache::Handle& out_handle );
