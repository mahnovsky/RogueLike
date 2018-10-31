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

stbtt_bakedchar cdata[96];
stbtt_fontinfo font;

Font::Font()
    :m_texture()
{}

Font::~Font()
{
}

void Font::init( const char* file )
{
    basic::Vector<char> data = basic::get_file_content( file );
    unsigned char *temp_bitmap = (unsigned char*)basic::mem_allocate( 512 * 512 );
    stbtt_BakeFontBitmap( (unsigned char*)data.get_raw(), 0, 32.0, temp_bitmap, 512, 512, 32, 96, cdata); // no guarantee this fits!
    basic::uint32 texture;
    stbtt_InitFont(&font, (unsigned char*)data.get_raw(), 0);

    for( int i = 0; i < 512 * 512; ++i )
    {
        if( temp_bitmap[i] > 0 )
        {
            LOG("%d", (int)temp_bitmap[i]);
        }
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    m_texture = std::move( Texture( 512, 512, texture ) );

    m_sp.set_size( 512, 512 );
    m_sp.set_anchor( 0, 0 );
    m_sp.init( &m_texture );

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

        if ( (ch >= 32) && (ch <= 127) )
        {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad( cdata, 512, 512, ch-32, &x, &y, &q, 1 );//1=opengl & d3d10+,0=d3d9
            /*
            glTexCoord2f(q.s0,q.t1); glVertex2f(q.x0,q.y0);
         glTexCoord2f(q.s1,q.t1); glVertex2f(q.x1,q.y0);
         glTexCoord2f(q.s1,q.t0); glVertex2f(q.x1,q.y1);
         glTexCoord2f(q.s0,q.t0); glVertex2f(q.x0,q.y1);
            */
            int x0, y0, x1, y1;
            stbtt_GetFontBoundingBox( &font, &x0, &y0, &x1, &y1 );

            vb.push( { { q.x0, 32.f-q.y0, 0.f }, {255, 255, 255, 255}, { q.s0, q.t1 } } );
            vb.push( { { q.x1, 32.f-q.y0, 0.f }, {255, 255, 255, 255}, { q.s1, q.t1 } } );
            vb.push( { { q.x1, 32.f-q.y1, 0.f }, {255, 255, 255, 255}, { q.s1, q.t0 } } );            
            vb.push( { { q.x0, 32.f-q.y1, 0.f }, {255, 255, 255, 255}, { q.s0, q.t0 } } ); 

            ib.push( 0 + offset * 4 );
            ib.push( 1 + offset * 4 );
            ib.push( 2 + offset * 4 );
            ib.push( 2 + offset * 4 );
            ib.push( 3 + offset * 4 );
            ib.push( 0 + offset * 4 );

            LOG( "y == %f", y );
        }
        ++offset;
    }

    m_quads.set_vertex_buffer( std::move( vb ) );
    m_quads.set_index_buffer( std::move( ib ) );
    m_quads.set_texture( &m_texture );
   
    if( !m_quads.is_initialized() )
    {
        m_quads.init();
    } 
    }
    render->draw( cam,&m_quads );
}

}
