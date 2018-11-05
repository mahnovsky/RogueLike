#include "text.hpp"

Text::Text()
    :m_font(nullptr)
    ,m_text()
{}

void Text::set_text( const char* text )
{
    m_text = text;
}

void Text::set_font( se::Font* font )
{
    m_font = font;
}

