#pragma once

#include "type_registration.hpp"
#include "generic_object_manager.hpp"
#include "defines.hpp"

class TextComponent : public IGenericObject
{
public:
    GENERIC_OBJECT_IMPL(TextComponent, NS_COMPONENT_TYPE);

    TextComponent( Entity* ent );
    TextComponent(Entity* ent, const char* font_file);
	~TextComponent();

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
