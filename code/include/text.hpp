#pragma once

#include "basic/string.hpp"
#include "font.hpp"

class Text 
{
public:
    Text();

    void set_text( const char* text );

    void set_font( se::Font* font );

	void set_font_size( float size );

	void set_position(const glm::vec3& pos);

	void set_scale(float scale);

	void draw(IRender* render, ICamera* cam);

private:
	void update();

private:
    se::Font* m_font;

    basic::String m_text;

    RenderObject m_render_object;

	float m_font_size;
};
