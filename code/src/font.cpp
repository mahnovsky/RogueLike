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

Font::Font()
{}

Font::~Font()
{}

void Font::init( const char* file )
{
    basic::Vector<char> data = basic::get_file_content( file );
    unsigned char *temp_bitmap = (unsigned char*)basic::mem_allocate( 512 * 512 );
    
    stbtt_BakeFontBitmap( (const unsigned char*)data.get_raw(), 0, 32.0, temp_bitmap, 512, 512, 32, 96, cdata); // no guarantee this fits!
    // can free ttf_buffer at this point
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
    // can free temp_bitmap at this point
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

}
