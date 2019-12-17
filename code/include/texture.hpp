#pragma once

#include "defines.hpp"
#include "resource_storage.hpp"

struct TextureRect
{
    float x;
    float y;
    float w;
    float h;
    basic::String name;
};

class Texture : public FileResource
{
public:
    Texture( ObjectManager* manager, const char* file );
    ~Texture( ) override;

    bool load( ResourceStorage* ) override;

    static Texture* create( ObjectManager* manager, const char* file );

    void init( basic::Image image );

    void init( basic::uint32 width,
               basic::uint32 height,
               basic::Vector< basic::uint8 > image_data,
               basic::uint32 cc );

    void init_font( basic::uint32 width,
                    basic::uint32 height,
                    basic::Vector< basic::uint8 > image_data );

    void set( basic::uint32 width, basic::uint32 height, basic::uint32 handle, basic::uint32 cc );

    void bind( ) const;

    void unbind( ) const;

    basic::uint32 get_width( ) const;

    basic::uint32 get_height( ) const;

    bool get_rect( const char* key, TextureRect& out_rect ) const;

    bool get_rect( basic::uint32 index, TextureRect& out_rect ) const;

	basic::uint32 get_handle() const;

private:
    basic::uint32 m_texture;
    basic::uint32 m_width;
    basic::uint32 m_height;
    basic::uint32 m_components;
    basic::Vector< TextureRect > m_rects;
};
