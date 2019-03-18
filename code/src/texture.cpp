#include "texture.hpp"
#include "GL/glew.h"
#include "basic/file.hpp"

#include "config.hpp"

#define TEXTURE_PATH "texture/"


Texture::Texture(ObjectManager *manager, const char* file )
    : FileResource( manager, file )
	, m_texture( 0 )
	, m_width(0)
	, m_height(0)
    , m_components(0)
{
}

Texture::~Texture( )
{
    if ( m_texture )
    {
        glDeleteTextures( 1, &m_texture );
    }
}

bool Texture::load(ResourceStorage* storage)
{
    basic::String name = get_name();
    basic::String path = TEXTURE_PATH;
    basic::String file = path + name;

    basic::uint32 index;
    if( name.find_last(index, '.') )
    {
        basic::String info_name = TEXTURE_PATH + name.get_substr(0, index);
        info_name += "conf";
        Config* conf = storage->get_resorce<Config>(info_name.get_cstr());
        basic::Vector<basic::String> frames = conf->get_values("frames");
        for(basic::uint32 i = 0; i < frames.get_size(); ++i)
        {
            LOG("%s", frames[i].get_cstr());
        }
    }

	basic::Vector<basic::uint8> data = basic::get_file_content(file.get_cstr());

	basic::Image img;
    if ( !data.is_empty() && basic::load_image( std::move( data ), img ) )
	{
        init(std::move(img));

		return true;
	}

	return false;
}

Texture* Texture::create(ObjectManager* manager, const char* file)
{
    Texture* tex = NEW_OBJ(Texture, manager, file);

	return tex;
}

void
Texture::init( basic::Image image )
{
    init( image.width, image.height, std::move( image.data ), image.components );
}

void
Texture::init( basic::uint32 width,
               basic::uint32 height,
               basic::Vector< basic::uint8 > image_data,
               basic::uint32 cc )
{
    m_width = width;
    m_height = height;
    m_components = cc;

    glGenTextures( 1, &m_texture );

    glBindTexture( GL_TEXTURE_2D, m_texture );

    GLuint format = (cc == 3 ? GL_RGB : GL_RGBA);
    GLint internalFormat = (format == GL_BGR ? GL_RGB8 : GL_RGBA8);

    glTexImage2D( GL_TEXTURE_2D,
                  0,
                  internalFormat,
                  static_cast<int>(width),
                  static_cast<int>(height),
                  0,
                  format,
                  GL_UNSIGNED_BYTE,
                  image_data.get_raw( ) );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

    glGenerateMipmap( GL_TEXTURE_2D );

	unbind();
}

void Texture::init_font(basic::uint32 width, basic::uint32 height, basic::Vector<basic::uint8> image_data)
{
	m_width = width;
	m_height = height;
	m_components = 1;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, GL_RED, GL_UNSIGNED_BYTE, image_data.get_raw());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unbind();
}

void Texture::set(basic::uint32 width, basic::uint32 height, basic::uint32 handle, basic::uint32 cc)
{
	m_width = width;
	m_height = height;
	m_components = cc;
	m_texture = handle;
}

void
Texture::bind( ) const
{
    glActiveTexture( GL_TEXTURE0 );

    glBindTexture( GL_TEXTURE_2D, m_texture );
}

void
Texture::unbind( ) const
{
    glBindTexture( GL_TEXTURE_2D, 0 );
}

basic::uint32
Texture::get_width( ) const
{
    return m_width;
}

basic::uint32
Texture::get_height( ) const
{
    return m_height;
}
