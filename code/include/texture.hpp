#pragma once

#include "basic/image.hpp"

class Texture
{
public:
    Texture();
    Texture( Texture&& t );
    Texture( basic::uint32 w,
             basic::uint32 h,
             basic::uint32 tex );
    ~Texture();

    Texture& operator = (Texture&& t);

    void init( basic::Image image );

    void init(  basic::uint32 width, 
                basic::uint32 height, 
                basic::Vector<char> image_data, 
                basic::ColorComponents cc );

    void bind() const;

    void unbind() const;

    basic::uint32 get_width() const;

    basic::uint32 get_height() const;

private:
    basic::uint32 m_texture;
    basic::uint32 m_width;
    basic::uint32 m_height;
    basic::ColorComponents m_components;
};
