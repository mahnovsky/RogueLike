#include "shader.hpp"
#include "GL/glew.h"
#include "basic/file.hpp"

Shader::Shader( )
    : m_shader_program( 0 )
{
}

Shader::~Shader( )
{
    if ( m_shader_program )
    {
        glDeleteProgram( m_shader_program );
    }
}

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

basic::uint32
Shader::get_uniform( const char* name ) const
{
    return glGetUniformLocation( m_shader_program, name );
}

void
Shader::bind( ) const
{
    glUseProgram( m_shader_program );
}

void
Shader::unbind( ) const
{
    glUseProgram( 0 );
}

bool
Shader::init( basic::Vector< basic::uint8 > vertex_data, basic::Vector< basic::uint8 > fragment_data )
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

    if ( !link_program( vshader, fshader ) )
    {
        LOG( "Failed link shader program" );
    }

    glDeleteShader( vshader );
    glDeleteShader( fshader );

    return true;
}

bool
Shader::compile( basic::Vector< basic::uint8 > data, basic::uint32 type, basic::uint32& out_id )
{
    GLuint index = glCreateShader( type );

    GLchar* data_ptr = (GLchar*)data.get_raw( );
    GLint gsize = data.get_size( );

    glShaderSource( index, 1, &data_ptr, &gsize );

    glCompileShader( index );

    out_id = index;

    return true;
}

bool
Shader::link_program( basic::uint32 vshader, basic::uint32 fshader )
{
    m_shader_program = glCreateProgram( );

    glAttachShader( m_shader_program, vshader );
    glAttachShader( m_shader_program, fshader );
    glLinkProgram( m_shader_program );

    if ( check_shader_link( m_shader_program ) )
    {
        return true;
    }

    return false;
}

bool
load_shader( ShaderCache& cache,
             const char* vshader_file,
             const char* fshader_file,
             ShaderCache::Handle& out_handle )
{
    basic::Vector< basic::uint8 > vertex_data = basic::get_file_content( vshader_file );
    basic::Vector< basic::uint8 > fragment_data = basic::get_file_content( fshader_file );

    if ( !vertex_data.is_empty( ) && !fragment_data.is_empty( ) )
    {
        Shader* shader = new Shader( );

        if ( shader->init( vertex_data, fragment_data ) )
        {
            basic::String shader_name = vshader_file;
            shader_name += "_";
            shader_name += fshader_file;

            cache.add( shader, shader_name.get_cstr( ), &out_handle );
        }
        else
        {
            return false;
        }
    }
    else
    {
        LOG( "Failed to load shader data vshader: %s fshader: %s", vshader_file, fshader_file );

        return false;
    }

    return true;
}
