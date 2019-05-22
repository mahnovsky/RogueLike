#include "material.hpp"

#include "render_common.hpp"
#include "shader.hpp"
#include "texture.hpp"

Material::Material( ShaderProgram* program, Texture* texture )
    : m_shader( program )
    , m_texture( texture )
{
    ASSERT( m_shader != nullptr );

    m_shader->retain( );
    if ( m_texture )
    {
        m_texture->retain( );
    }
}

Material::~Material( )
{
    SAFE_RELEASE( m_shader );
    SAFE_RELEASE( m_texture );
}

void
Material::enable( ) const
{
    m_shader->bind( );
    if ( m_texture )
    {
        m_texture->bind( );
    }
}

void
Material::disable( ) const
{
    if ( m_texture )
    {
        m_texture->unbind( );
    }

    m_shader->unbind( );
}

void
Material::set_uniform( const char* name, const glm::vec2& v )
{
    basic::int32 pos = m_shader->get_uniform( name );

    glUniform2f( pos, v.x, v.y );
}

void
Material::set_uniform( const char* name, const basic::Color& color ) const
{
    basic::int32 upos = m_shader->get_uniform( name );

    glm::vec4 c{static_cast< float >( color.red ) / 255,
                static_cast< float >( color.blue ) / 255,
                static_cast< float >( color.green ) / 255,
                static_cast< float >( color.alpha ) / 255};

    glUniform4fv( upos, 1, glm::value_ptr( c ) );
}

void
Material::set_uniform( const char* name, const glm::mat4& mat ) const
{
    basic::int32 upos = m_shader->get_uniform( name );

    glUniformMatrix4fv( upos, 1, GL_FALSE, glm::value_ptr( mat ) );
}

const Texture*
Material::get_texture( ) const
{
    return m_texture;
}
