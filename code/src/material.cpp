#include "material.hpp"

#include "shader.hpp"
#include "texture.hpp"
#include "render_common.hpp"

Material::Material(ShaderProgram* program, Texture* texture)
    :m_shader(program)
    ,m_texture(texture)
{
    ASSERT( m_shader != nullptr );

    m_shader->retain();
    if(m_texture)
    {
        m_texture->retain();
    }
}

Material::~Material()
{
    if( m_shader )
    {
        m_shader->release();
    }
    if(m_texture)
    {
        m_texture->release();
    }
}

void Material::enable() const
{
    m_shader->bind();
    if(m_texture)
    {
        m_texture->bind();
    }
}

void Material::disable() const
{
    m_shader->unbind();
    if(m_texture)
    {
        m_texture->unbind();
    }
}

void Material::load_color( const char *name, const basic::Color &color ) const
{
    basic::int32 upos = m_shader->get_uniform( name );

    glm::vec4 c{ color.red, color.blue, color.green, color.alpha };

    c = glm::normalize( c );

    glUniform4fv( upos, 4, glm::value_ptr( c ) );
}

void Material::load_matrix( const char *name, const glm::mat4 &mat ) const
{
    basic::int32 upos = m_shader->get_uniform( name );

    glUniformMatrix4fv( upos, 1, GL_TRUE, glm::value_ptr( mat ) );
}
