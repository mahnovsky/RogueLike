#pragma once

#include "basic/types.hpp"

namespace se
{

class Font
{
public:
    Font();
    ~Font();

    void init( const char* file );

private:
    basic::uint32 m_texture; 
    RenderObject m_quads;
};

}

