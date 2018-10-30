#pragma once 

#include "texture.hpp"

using TextureHandle = size_t;

class TextureCache
{
public:
    static const TextureHandle INVALID_HANDLE = 0xFFFFFFFF;

    struct TextureInfo
    {
        basic::String name;
        TextureHandle handle;
        Texture* texture;
    };

    TextureCache();
    ~TextureCache();

    bool load_texture( const char* file, TextureHandle& out_handle );

    Texture* get_texture( TextureHandle handle ) const;

    TextureHandle get_texture_handle( const char* name );

private:
    basic::Vector<TextureInfo> m_textures;
    size_t m_handle_counter;
};

