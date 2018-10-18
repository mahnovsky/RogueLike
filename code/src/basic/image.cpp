#include "basic/image.hpp"
#include "basic/util_functions.hpp"

namespace basic
{

static const size_t MIN_IMAGE_SIZE = 64;

static const size_t BMP_HEADER_SIZE = 54;

bool load_bmp( Vector<char> data, Image& out_image )
{
    ASSERT( data.get_size() > BMP_HEADER_SIZE );

    uint8 header[BMP_HEADER_SIZE] = {};

    mem_copy( header, data.get_raw(), BMP_HEADER_SIZE );    

    uint32 data_offset = *(uint32*)&(header[0x0A]);
    uint32 image_size = *(uint32*)&(header[0x22]);
    uint32 width = *(uint32*)&(header[0x12]);
    uint32 height = *(uint32*)&(header[0x16]);

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
        out_image.components = ColorComponents::BGR;

        return true;
    }

    return false;
}


bool load_image( Vector<char> data, Image& out_image )
{
    if( data.get_size() < MIN_IMAGE_SIZE )
    {
        return false;
    }

    if( data[0] == 'B' && data[1] == 'M' )
    {
        return load_bmp( std::move( data ), out_image );
    }

    return false;
}

}
