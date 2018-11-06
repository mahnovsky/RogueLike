#include "shader.hpp"
#include "GL/glew.h"

Shader::Shader()
    :m_shader_program(0)
{}

void
check_shader( GLuint shader )
{
    GLint success;
    GLchar infoLog[ 512 ];
    glGetShaderiv( shader, GL_COMPILE_STATUS, &success );

    if ( success )
    {
        LOG( "Shader compiled successfuly %u", shader );
    }
    else
    {
        glGetShaderInfoLog( shader, 512, NULL, infoLog );
        LOG( "ERROR vshader compilation failed  %s", infoLog );
    }
 }

bool
check_shader_link( GLuint shader )
{
    GLint success;
    GLchar infoLog[ 512 ];
    glGetProgramiv( shader, GL_LINK_STATUS, &success );

    if ( success )
    {
        LOG( "Shader linked successfuly %u", shader );
    }
    else
    {
        glGetProgramInfoLog( shader, 512, NULL, infoLog );
        LOG( "Shader link failed %s", infoLog );
    }

    return success > 0;
}

basic::uint32 Shader::get_uniform( const char* name ) const
{
    return glGetUniformLocation( m_shader_program, name );
}

void Shader::bind() const
{
    glUseProgram( m_shader_program );
}
    
void Shader::unbind() const
{
    glUseProgram( 0 );
}

bool Shader::init( basic::Vector<char> vertex_data, basic::Vector<char> fragment_data )
{
    GLuint vshader = 0;
    if ( compile( std::move( vertex_data ), GL_VERTEX_SHADER, vshader ) )
    {
        LOG( "vshader %u", vshader );

        check_shader( vshader );
    }
    else
    {
        LOG( "failed vshader %u", vshader );

        return false;
    }

    GLuint fshader = 0;
    if ( compile( std::move( fragment_data ), GL_FRAGMENT_SHADER, fshader ) )
    {
        LOG( "fshader %u", fshader );

        check_shader( fshader );
    }
    else
    {
        LOG( "failed fshader %u", fshader );

        return false;
    }

    if( !link_program( vshader, fshader ) )
    {
        LOG( "Failed link shader program" );
    }

    glDeleteShader( vshader );
    glDeleteShader( fshader );
}

bool Shader::compile( basic::Vector<char> data, basic::uint32 type, basic::uint32& out_id )
{
    GLuint index = glCreateShader( type );

    GLchar* data_ptr = (GLchar*)data.get_raw();
    GLint gsize = data.get_size();

    glShaderSource( index, 1, &data_ptr, &gsize );

    glCompileShader( index );

    out_id = index;

	return true;
}
   
bool Shader::link_program( basic::uint32 vshader, basic::uint32 fshader )
{
    m_shader_program = glCreateProgram( );

    glAttachShader( m_shader_program, vshader );
    glAttachShader( m_shader_program, fshader );
    glLinkProgram( m_shader_program );

    if( check_shader_link( m_shader_program ) )
    {
        return true;
    }

    return false;
}

