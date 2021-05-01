#include "text_component.hpp"

#include "font.hpp"
#include "render_common.hpp"
#include "transform.hpp"

TextComponent::TextComponent(Entity* ent)
    : m_font( nullptr )
	, m_font_file("arial.ttf")
    , m_text()
    , m_font_size( 32.f )
{
}

TextComponent::TextComponent(Entity* ent, const char * font_file)
	: m_font(nullptr)
	, m_font_file(font_file)
    , m_text()
	, m_font_size(32.f)
{

}

TextComponent::~TextComponent()
{
}

void TextComponent::init( core::ResourceStorage * storage )
{
	m_font = storage->get_resorce<se::Font>(m_font_file.c_str());
	if (m_font)
	{
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

void TextComponent::set_font( std::shared_ptr<se::Font> font )
{
    m_font = font;
	if (m_font)
	{
		update();
	}
}

void TextComponent::set_font_size( float size )
{
    m_font_size = size;
    update( );
}

void TextComponent::set_position( const glm::vec3& pos )
{
}

void TextComponent::set_scale( float scale )
{
}

void TextComponent::draw( ICamera* cam )
{
}

void TextComponent::update( )
{
    if ( m_font && !m_text.empty( ) )
    {
    }
}
