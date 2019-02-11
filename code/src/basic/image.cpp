#include "basic/image.hpp"
#include "basic/util_functions.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

bool load_image( basic::Vector<basic::uint8> data, basic::Image& out_image )
{
    int x, y, components;
    stbi_uc* unpack_image = stbi_load_from_memory( data.get_raw(),
                                                   static_cast<int>(data.get_size()),
                                                   &x, &y, &components, 0 );


    if( unpack_image )
    {
        out_image.data.init( unpack_image, static_cast<basic::uint32>(x * y * components) );
        out_image.width = static_cast<basic::uint32>(x);
        out_image.height = static_cast<basic::uint32>(y);
        out_image.components = static_cast<basic::uint32>(components);
    }

    return true;
}

namespace basic
{

static const size_t MIN_IMAGE_SIZE = 64;

static const size_t BMP_HEADER_SIZE = 54;

bool load_bmp( Vector<uint8> data, Image& out_image )
{
    ASSERT( data.get_size() > BMP_HEADER_SIZE );

    uint8 header[BMP_HEADER_SIZE] = {};

    mem_copy( header, data.get_raw(), BMP_HEADER_SIZE );    

    uint32 data_offset = *reinterpret_cast<uint32*>(&header[0x0A]);
    uint32 image_size = *reinterpret_cast<uint32*>(&header[0x22]);
    uint32 width = *reinterpret_cast<uint32*>(&header[0x12]);
    uint32 height = *reinterpret_cast<uint32*>(&header[0x16]);

    if( image_size == 0 )
    {
        image_size = width * height * 3;
    }
    if( data_offset == 0 )
    {
        data_offset = BMP_HEADER_SIZE;
    }

    if( (image_size + BMP_HEADER_SIZE) <= data.get_size() )
    {
        out_image.data.init( data.get_raw() + data_offset, image_size );
        out_image.width = width;
        out_image.height = height;
        out_image.components = 3;

        return true;
    }

    return false;
}


bool load_image(Vector<uint8> data, Image& out_image )
{
    if( data.get_size() < MIN_IMAGE_SIZE )
    {
        return false;
    }

    return ::load_image( data, out_image );
}

}
