#include "text_component.hpp"

#include "font.hpp"
#include "render_common.hpp"
#include "transform.hpp"

TextComponent::TextComponent(Entity* ent)
    : m_font( nullptr )
	, m_font_file("arial.ttf")
    , m_text()
    , m_render_node( nullptr )
    , m_font_size( 32.f )
{
}

TextComponent::TextComponent(Entity* ent, const char * font_file)
	: m_font(nullptr)
	, m_font_file(font_file)
    , m_text()
    , m_render_node( nullptr )
	, m_font_size(32.f)
{

}

TextComponent::~TextComponent()
{
	if (m_font)
	{
		m_font->deref();
	}
    if(m_render_node)
    {
        RenderNode::remove_node( m_render_node );
    }
}

void TextComponent::init( ResourceStorage * storage )
{
	m_font = storage->get_resorce<se::Font>(m_font_file.get_cstr());
	if (m_font)
	{
		m_font->ref();
		
		update();
	}
}

void TextComponent::set_text( const char* text )
{
	if (m_text == text)
	{
		return;
	}
    m_text = text;
    update( );
}

void TextComponent::set_font( se::Font* font )
{
    m_font = font;
    update( );
}

void TextComponent::set_font_size( float size )
{
    m_font_size = size;
    update( );
}

void TextComponent::set_position( const glm::vec3& pos )
{
    if( m_render_node )
    {
        m_render_node->get_transform()->set_position( pos );
    }
}

void TextComponent::set_scale( float scale )
{
   m_render_node->get_transform()->set_scale( {scale, scale, scale} );
}

void TextComponent::draw( ICamera* cam )
{
    if(m_render_node)
    {
        m_render_node->set_camera(cam);
    }

    m_render_node->draw_node(  );
}

void TextComponent::update( )
{
    if ( m_font && !m_text.is_empty( ) )
    {
        if( !m_render_node )
        {
            m_render_node = m_font->create_text_node();
            m_render_node->get_transform()->set_scale({1.f, -1.f, 1.f});
        }
        glm::vec2 size;
        m_font->update( m_text.get_cstr(), m_render_node, size );
    }
}
