#include "font.hpp"
#include "GL/glew.h"

#include "texture.hpp"
#include "shader.hpp"
#include "render_object.hpp"

namespace
{
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"
}

namespace se
{
Font::Font( )
    : m_texture( nullptr )
    , m_height( 32.f )
    , m_cdata( basic::mem_alloc( 96 * sizeof( stbtt_bakedchar ) ) )
	, m_shader(nullptr)
{
}

Font::~Font( )
{
    basic::mem_free( m_cdata );
	if (m_texture)
	{
		m_texture->release();
	}
}

bool
Font::init( const char* file, ShaderProgram* shader )
{
	basic::String path = "fonts/";
	path += file;
    basic::Vector< basic::uint8 > data = basic::get_file_content( path.get_cstr() );

    if ( data.is_empty( ) )
    {
        return false;
    }

    m_shader = shader;

    const int tw = 512;
	const int th = 512;

	basic::Vector<basic::uint8> bitmap;
	bitmap.resize(tw * th);
    
    stbtt_BakeFontBitmap( data.get_raw( ),
                          0,
                          m_height,
                          bitmap.get_raw(),
                          tw,
                          th,
                          32,
                          96,
                          static_cast<stbtt_bakedchar*>( m_cdata ) );

	basic::String name = "bitmap_";

    m_texture = new Texture( (name + file).get_cstr() );
	m_texture->init_font(tw, th, std::move(bitmap));
	m_texture->retain();

    return true;
}

void
Font::generate( const char* text, float height, RenderObject& out_object )
{
    if ( !text )
    {
        return;
    }

    VertexBuffer vb;
    IndexBuffer ib;

    float x = 0;
    float y = 0;
    basic::uint16 offset = 0;
    while ( *( text + offset ) )
    {
        char ch = *( text + offset );
        if ( !ch )
        {
            break;
        }

        if ( ch >= 32 )
        {
            stbtt_aligned_quad q;
            int w = static_cast<int>( m_texture->get_width( ) );
            int h = static_cast<int>( m_texture->get_height( ) );

            stbtt_GetBakedQuad( static_cast<stbtt_bakedchar*>(m_cdata), w, h, ch - 32, &x, &y, &q, 1 );

            auto xmin = q.x0;
            auto xmax = q.x1;
            auto ymin = -q.y1;
            auto ymax = -q.y0;
            glm::vec3 p0 = {xmin, ymin, 0};
            glm::vec3 p1 = {xmin, ymax, 0};
            glm::vec3 p2 = {xmax, ymax, 0};
            glm::vec3 p3 = {xmax, ymin, 0};
            glm::vec2 t0 = {q.s0, q.t1};
            glm::vec2 t1 = {q.s0, q.t0};
            glm::vec2 t2 = {q.s1, q.t0};
            glm::vec2 t3 = {q.s1, q.t1};
            /* https://github.com/0xc0dec/demos/blob/master/src/StbTrueType.cpp*/
            vb.push( {p0, {255, 255, 255, 255}, t0} );
            vb.push( {p1, {255, 255, 255, 255}, t1} );
            vb.push( {p2, {255, 255, 255, 255}, t2} );
            vb.push( {p3, {255, 255, 255, 255}, t3} );

            ib.push( 0 + offset * 4 );
            ib.push( 1 + offset * 4 );
            ib.push( 2 + offset * 4 );
            ib.push( 0 + offset * 4 );
            ib.push( 2 + offset * 4 );
            ib.push( 3 + offset * 4 );
        }
        ++offset;
    }

    out_object.set_vertex_buffer( std::move( vb ) );
    out_object.set_index_buffer( std::move( ib ) );

    if ( !out_object.is_initialized( ) )
    {
        out_object.set_texture( m_texture );
        out_object.set_shader( m_shader );
    }

    out_object.init( );
}
}
