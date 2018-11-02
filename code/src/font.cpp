#include "font.hpp"
namespace 
{
extern "C"
{
#include <stdio.h>
#define STB_TRUETYPE_IMPLEMENTATION  
#include "stb_truetype.h"
}
}

namespace se
{

Font::Font()
    :m_texture()
    ,m_height( 22.f )
    ,m_cdata( basic::mem_allocate( 96 * sizeof( stbtt_bakedchar )) )
{
}

Font::~Font()
{
    basic::mem_free( m_cdata );
}

void Font::init( const char* file )
{
    basic::Vector<char> data = basic::get_file_content( file );
    float tw = 512;
    float th = 512;

    unsigned char *temp_bitmap = (unsigned char*)basic::mem_allocate( tw * th );
    stbtt_BakeFontBitmap( (unsigned char*)data.get_raw(), 0, m_height, temp_bitmap, tw, th, 32, 96, (stbtt_bakedchar*)m_cdata);
    basic::uint32 texture;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, tw, th, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    m_texture = std::move( Texture( tw, th, texture ) );


    basic::mem_free( temp_bitmap );
}

void Font::draw( ICamera* cam, IRender* render, const char* text )
{
    if( !m_quads.is_initialized() )
    {
    VertexBuffer vb;
    IndexBuffer ib;
    m_quads.get_transform()->set_position( { 100, 100, 0} );
    float x = 0;
    float y = 0;
    int offset = 0;
    while (*(text + offset) ) 
    {
        char ch = *(text + offset);
        if( !ch ) break;

        if ( (ch >= 32) && (ch <= 127) )
        {
            stbtt_aligned_quad q;
            basic::uint32 w = m_texture.get_width();
            basic::uint32 h = m_texture.get_height();

            stbtt_GetBakedQuad( (stbtt_bakedchar*)m_cdata, w, h, ch-32, &x, &y, &q, 1 );

            LOG( "y == %f", q.y0 );
            float delta = q.y0 + m_height;
            q.y0 -= (delta - m_height);
            q.y1 -= (delta - m_height);
            vb.push( { { q.x0,  q.y0, 0.f }, {255, 255, 255, 255}, { q.s0, q.t1 } } );
            vb.push( { { q.x1,  q.y0, 0.f }, {255, 255, 255, 255}, { q.s1, q.t1 } } );
            vb.push( { { q.x1,  q.y1, 0.f }, {255, 255, 255, 255}, { q.s1, q.t0 } } );
            vb.push( { { q.x0,  q.y1, 0.f }, {255, 255, 255, 255}, { q.s0, q.t0 } } ); 

            ib.push( 0 + offset * 4 );
            ib.push( 1 + offset * 4 );
            ib.push( 2 + offset * 4 );
            ib.push( 2 + offset * 4 );
            ib.push( 3 + offset * 4 );
            ib.push( 0 + offset * 4 );

            LOG( "y == %f", q.y0 );
        }
        ++offset;
    }

    m_quads.set_vertex_buffer( std::move( vb ) );
    m_quads.set_index_buffer( std::move( ib ) );
    m_quads.set_texture( &m_texture );
   
        m_quads.init();
    }
    render->draw( cam, &m_quads );
}

}
