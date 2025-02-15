#include "texture.hpp"
#include "GL/glew.h"
#include "file.hpp"

#include "config.hpp"

#define TEXTURE_PATH "textures/"

Texture::Texture( const char* file )
    : FileResource( file )
    , m_texture( 0 )
    , m_width( 0 )
    , m_height( 0 )
    , m_components( 0 )
{
}

Texture::~Texture( )
{
    if ( m_texture )
    {
        glDeleteTextures( 1, &m_texture );
    }
}

bool Texture::load( core::ResourceStorage* storage )
{
	if (m_texture != 0)
	{
		return true;
	}

    const std::string path = TEXTURE_PATH;
    const std::string file = path + get_file_name().data();

    size_t index = file.find_last_of('.');
	if (index != std::string::npos)
	{
		std::string info_name = file.substr(0, index);
		info_name += ".conf";

		const auto conf = storage->get_resource< Config >(info_name.c_str());

		if (conf)
		{
			const basic::JsonObject* frames = conf->get_values("frames");
			auto frames_array = frames->to_array();
			for (const basic::JsonObject* frame : frames_array)
			{
				TextureRect rect;
				const bool success =
					frame->get("x", rect.rect.pos.x) &&
                    frame->get("y", rect.rect.pos.y) && 
                    frame->get("w", rect.rect.size.x) && 
                    frame->get("h", rect.rect.size.y) &&
					frame->get("name", rect.name);

                if (success)
                {
                    m_rects.push_back(rect);
                }
			}
		}
	}

    std::vector< uint8_t > data = core::get_file_content( file.c_str( ) );

    basic::Image img;
    if ( !data.empty( ) && basic::load_image( std::move( data ), img ) )
    {
        init( std::move( img ) );

        return true;
    }

    return false;
}

Texture* Texture::create( const char* file )
{
    Texture* tex = NEW_OBJ( Texture, file );

    return tex;
}

void Texture::init( basic::Image image )
{
    init( image.width, image.height, std::move( image.data ), image.components );
}

void Texture::init(uint32_t width, uint32_t height, std::vector< uint8_t > image_data, uint32_t cc)
{
    m_width = width;
    m_height = height;
    m_components = cc;

    glGenTextures( 1, &m_texture );

    glBindTexture( GL_TEXTURE_2D, m_texture );

    const GLuint format = ( cc == 3 ? GL_RGB : GL_RGBA );
    const GLint internalFormat = ( format == GL_BGR ? GL_RGB8 : GL_RGBA8 );

    glTexImage2D( GL_TEXTURE_2D,
                  0,
                  internalFormat,
                  static_cast< int >( width ),
                  static_cast< int >( height ),
                  0,
                  format,
                  GL_UNSIGNED_BYTE,
                  image_data.data( ) );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

    glGenerateMipmap( GL_TEXTURE_2D );

    unbind( );
}

void Texture::init_font( basic::uint32 width,
                    basic::uint32 height,
                    std::vector< uint8_t > image_data )
{
    m_width = width;
    m_height = height;
    m_components = 1;

    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

    glGenTextures( 1, &m_texture );
    glBindTexture( GL_TEXTURE_2D, m_texture );
    glTexImage2D( GL_TEXTURE_2D,
                  0,
                  GL_RED,
                  m_width,
                  m_height,
                  0,
                  GL_RED,
                  GL_UNSIGNED_BYTE,
                  image_data.data( ) );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    unbind( );
}

void Texture::set( basic::uint32 width, basic::uint32 height, basic::uint32 handle, basic::uint32 cc )
{
    m_width = width;
    m_height = height;
    m_components = cc;
    m_texture = handle;
}

void Texture::bind( ) const
{
    glActiveTexture( GL_TEXTURE0 );

    glBindTexture( GL_TEXTURE_2D, m_texture );
}

void Texture::unbind( ) const
{
    glBindTexture( GL_TEXTURE_2D, 0 );
}

uint32_t Texture::get_width() const
{
    return m_width;
}

uint32_t Texture::get_height() const
{
    return m_height;
}

bool Texture::get_rect( const char* key, TextureRect& out_rect ) const
{
    for ( auto& rect : m_rects )
    {
        if ( rect.name == key )
        {
            out_rect = rect;
            return true;
        }
    }
    return false;
}

bool Texture::get_rect(uint32_t index, TextureRect& out_rect) const
{
	if (index < m_rects.size())
	{
		out_rect = m_rects[index];
		return true;
	}
	return false;
}

uint32_t Texture::get_handle() const
{
	return m_texture;
}
