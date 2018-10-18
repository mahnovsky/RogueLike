#pragma once

#include "basic/image.hpp"

class Texture
{
public:
    Texture();
    ~Texture();

    void init( basic::Image image );

    void init(  basic::uint32 width, 
                basic::uint32 height, 
                basic::Vector<char> image_data, 
                basic::ColorComponents cc );

    void bind() const;

    void unbind() const;

private:
    basic::uint32 m_texture;
    basic::uint32 m_width;
    basic::uint32 m_height;
    basic::ColorComponents m_components;
};