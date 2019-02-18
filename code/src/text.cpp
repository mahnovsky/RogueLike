#include "text.hpp"

#include "font.hpp"

Text::Text( )
    : m_font( nullptr )
	, m_font_file("arial.ttf")
    , m_text()
    , m_render_object( )
    , m_font_size( 32.f )
{
    m_render_object.get_transform( )->set_forward( {0.f, 0.f, 1.f} );
}

Text::Text(const char * font_file)
	: m_font(nullptr)
	, m_font_file(font_file)
	, m_text()
	, m_render_object()
	, m_font_size(32.f)
{

}

Text::~Text()
{
	if (m_font)
	{
		m_font->release();
	}
}

void Text::init(ResourceStorage * storage)
{
	m_font = storage->get_resorce<se::Font>(m_font_file.get_cstr());
	if (m_font)
	{
		m_font->retain();
		
		update();
	}
}

void
Text::set_text( const char* text )
{
	if (m_text == text)
	{
		return;
	}
    m_text = text;
    update( );
}

void
Text::set_font( se::Font* font )
{
    m_font = font;
    update( );
}

void
Text::set_font_size( float size )
{
    m_font_size = size;
    update( );
}

void
Text::set_position( const glm::vec3& pos )
{
    m_render_object.get_transform( )->set_position( pos );
}

void
Text::set_scale( float scale )
{
    m_render_object.get_transform( )->set_scale( {scale, scale, scale} );
}

void
Text::draw( IRender* render, ICamera* cam )
{
    m_render_object.draw( render, cam );
}

void
Text::update( )
{
    if ( m_font && !m_text.is_empty( ) )
    {
        m_font->generate( m_text.get_cstr( ), m_font_size, m_render_object );
    }
}
