#pragma once

#include "defines.hpp"

class Text
{
public:
    Text( );
	Text(const char* font_file);
	~Text();

	void init(ResourceStorage* storage);

    void set_text( const char* text );

    void set_font( se::Font* font );

    void set_font_size( float size );

    void set_position( const glm::vec3& pos );

    void set_scale( float scale );

    void draw( ICamera* cam );

private:
    void update( );

private:
	se::Font* m_font;

	basic::String m_font_file;

    basic::String m_text;

    RenderNode* m_render_node;

    float m_font_size;
};
