#include "font.hpp"
#include "GL/glew.h"
#include "basic/file.hpp"

namespace
{
extern "C" {
#include <stdio.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
}
}

namespace se
{
Font::Font( )
    : m_texture( )
    , m_height( 32.f )
    , m_cdata( basic::mem_allocate( 96 * sizeof( stbtt_bakedchar ) ) )
{
}

Font::~Font( )
{
    basic::mem_free( m_cdata );
}

bool
Font::init( const char* file, Shader* shader )
{
    basic::Vector< char > data = basic::get_file_content( file );
    if ( data.is_empty( ) )
    {
        return false;
    }

    m_shader = shader;

    float tw = 512;
    float th = 512;

    unsigned char* temp_bitmap = (unsigned char*)basic::mem_allocate( tw * th );
    stbtt_BakeFontBitmap( (unsigned char*)data.get_raw( ),
                          0,
                          m_height,
                          temp_bitmap,
                          tw,
                          th,
                          32,
                          96,
                          (stbtt_bakedchar*)m_cdata );
    basic::uint32 texture;

    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RED, tw, th, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    m_texture = std::move( Texture( tw, th, texture ) );

    basic::mem_free( temp_bitmap );

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
    int offset = 0;
    while ( *( text + offset ) )
    {
        char ch = *( text + offset );
        if ( !ch )
            break;

        if ( ( ch >= 32 ) && ( ch <= 127 ) )
        {
            stbtt_aligned_quad q;
            basic::uint32 w = m_texture.get_width( );
            basic::uint32 h = m_texture.get_height( );

            stbtt_GetBakedQuad( (stbtt_bakedchar*)m_cdata, w, h, ch - 32, &x, &y, &q, 1 );

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

            LOG( "y == %f", q.y0 );
        }
        ++offset;
    }

    out_object.set_vertex_buffer( std::move( vb ) );
    out_object.set_index_buffer( std::move( ib ) );
    out_object.set_texture( &m_texture );
    out_object.set_shader( m_shader );

    if ( !out_object.is_initialized( ) )
    {
        out_object.init( );
    }
}
}
