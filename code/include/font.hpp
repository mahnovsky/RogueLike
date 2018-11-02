#pragma once

#include "basic/types.hpp"
#include "sprite.hpp"

namespace se
{

struct bakedchar;

class Font
{
public:
    Font();
    ~Font();

    void init( const char* file );

    void draw( ICamera* cam, IRender* render, const char* text );

private:
    Texture m_texture; 
    RenderObject m_quads;
    float m_height;
    void* m_cdata;
};

}

