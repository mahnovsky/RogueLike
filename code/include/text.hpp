#pragma once

#include "basic/string.hpp"
#include "font.hpp"

class Text 
{
public:
    Text();

    void set_text( const char* text );

    void set_font( se::Font* font );

private:
    se::Font* m_font;

    basic::String m_text;

    RenderObject m_render_object;
};
