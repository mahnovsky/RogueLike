#include "image.hpp"
#include "debug.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <vector>
namespace stb_impl
{
	bool load_image(std::vector<uint8_t> data, basic::Image& out_image)
	{
		int x, y, components;
		const stbi_uc* unpack_image = stbi_load_from_memory(data.data(),
			static_cast<int>(data.size()),
			&x, &y, &components, 0);

		if (unpack_image)
		{
			out_image.data.resize(x * y * components);
			std::copy_n(unpack_image, (x * y * components), out_image.data.begin());
			//out_image.data = std::vector<uint8_t>( unpack_image, static_cast<basic::uint32>(x * y * components) );
			out_image.width = static_cast<basic::uint32>(x);
			out_image.height = static_cast<basic::uint32>(y);
			out_image.components = static_cast<basic::uint32>(components);
		}

		return true;
	}
} // namespace stb_impl

namespace basic
{

static constexpr size_t MIN_IMAGE_SIZE = 64;
static constexpr size_t BMP_HEADER_SIZE = 54;

bool load_bmp( std::vector<uint8> data, Image& out_image )
{
    ASSERT( data.size() > BMP_HEADER_SIZE );

    uint8 header[BMP_HEADER_SIZE] = {};

    mem_copy( header, data.data(), BMP_HEADER_SIZE );    

    uint32 data_offset = *reinterpret_cast<uint32*>(&header[0x0A]);
    uint32 image_size = *reinterpret_cast<uint32*>(&header[0x22]);
	const uint32 width = *reinterpret_cast<uint32*>(&header[0x12]);
	const uint32 height = *reinterpret_cast<uint32*>(&header[0x16]);

    if( image_size == 0 )
    {
        image_size = width * height * 3;
    }
    if( data_offset == 0 )
    {
        data_offset = BMP_HEADER_SIZE;
    }

    if( (image_size + BMP_HEADER_SIZE) <= data.size() )
    {
        out_image.data.resize(image_size);
        std::copy_n(data.data(), image_size, out_image.data.begin());
        out_image.width = width;
        out_image.height = height;
        out_image.components = 3;

        return true;
    }

    return false;
}


bool load_image(std::vector<uint8_t> data, Image& out_image )
{
    if( data.size() < MIN_IMAGE_SIZE )
    {
        return false;
    }

    return stb_impl::load_image( std::move( data ), out_image );
}

}
