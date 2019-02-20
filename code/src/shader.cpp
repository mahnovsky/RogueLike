#include "shader.hpp"

#include "GL/glew.h"
#include "basic/file.hpp"
#include "config.hpp"

ShaderProgram::ShaderProgram(const char *file)
    : FileResource ( file )
    , m_shader_program( 0 )
{
}

ShaderProgram::~ShaderProgram( )
{
    if( m_vertex_shader )
    {
        m_vertex_shader->release();
        m_vertex_shader = nullptr;
    }

    if( m_fragment_shader )
    {
        m_fragment_shader->release();
        m_fragment_shader = nullptr;
    }

    if ( m_shader_program )
    {
        glDeleteProgram( m_shader_program );
    }
}

bool ShaderProgram::load( ResourceStorage *storage )
{
    Config* config = storage->get_resorce<Config>( "shaders/programs.conf" );

    if( config )
    {
        basic::String name = get_name();
        basic::Vector<basic::String> values = config->get_values( name.get_cstr() );

        basic::String vertex_file;
        basic::String fragment_file;

        for(basic::uint32 i = 0; i < values.get_size(); ++i)
        {
            basic::String& line = values[i];
            if( line.ends_of( ".vs" ) )
            {
                vertex_file = line;
            }
            else if( line.ends_of( ".fs" ) )
            {
                fragment_file = line;
            }
        }

        if( !vertex_file.is_empty() && !fragment_file.is_empty() )
        {
            BaseShader* vertex = storage->get_resorce<BaseShader>( vertex_file.get_cstr() );
            BaseShader* fragment = storage->get_resorce<BaseShader>( fragment_file.get_cstr() );

            return init( vertex, fragment );
        }
    }

    return false;
}

void print_shader_info( GLuint shader )
{
    GLchar infoLog[ 512 ];
    glGetShaderInfoLog( shader, 512, nullptr, infoLog );
    LOG( "Shader info: %s", infoLog );
}

bool
is_shader_compiled( GLuint shader )
{
    GLint success = 0;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &success );

    return success > 0;
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
        glGetProgramInfoLog( shader, 512, nullptr, infoLog );
        LOG( "Shader link failed %s", infoLog );
    }

    return success > 0;
}

basic::int32
ShaderProgram::get_uniform( const char* name ) const
{
    return glGetUniformLocation( m_shader_program, name );
}

ShaderProgram *ShaderProgram::create(const char *file)
{
    ShaderProgram* res = new ShaderProgram( file );

    return res;
}

void
ShaderProgram::bind( ) const
{
    glUseProgram( m_shader_program );
}

void
ShaderProgram::unbind( ) const
{
    glUseProgram( 0 );
}

bool
ShaderProgram::init(BaseShader *vertex, BaseShader *fragment )
{
    if( vertex && vertex->is_valid() &&
        fragment && fragment->is_valid() &&
        vertex->get_tag() == GL_VERTEX_SHADER &&
        fragment->get_tag() == GL_FRAGMENT_SHADER )
    {
        if ( link_program( vertex->get_handle(), fragment->get_handle() ) )
        {
            m_vertex_shader = vertex;
            m_fragment_shader = fragment;

            vertex->retain();
            fragment->retain();

            return true;
        }
    }
    LOG( "Failed link shader program" );

    return false;
}

bool
ShaderProgram::link_program( basic::uint32 vshader, basic::uint32 fshader )
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

static GLuint compile( basic::Vector<basic::uint8> data, basic::uint32 type )
{
    GLuint handle = glCreateShader( type );
    data.push(0);
    char* data_ptr = reinterpret_cast<GLchar*>( data.get_raw( ) );
    GLint gsize = static_cast<GLint>( data.get_size( ) );

    glShaderSource( handle, 1, &data_ptr, &gsize );
    glCompileShader( handle );

    return handle;
}

BaseShader::BaseShader( basic::uint32 type, const char* file )
    :FileResource (file)
    ,m_handle(0)
{
    bool is_valid = type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER;
    ASSERT( is_valid );

    set_tag( type );
}

BaseShader::~BaseShader()
{
    glDeleteShader( m_handle );
}

bool BaseShader::load(ResourceStorage *)
{
    if( is_valid() )
    {
        return true;
    }

    basic::String file = get_name();
    ASSERT( !file.is_empty() );

    basic::Vector<basic::uint8> data = basic::get_file_content( file.get_cstr() );

    if( !data.is_empty() )
    {
        m_handle = compile( data, static_cast<basic::uint32>( get_tag() ) );

        bool valid = is_valid();
        if( !valid )
        {
            print_shader_info( m_handle );
        }
        return valid;
    }

    return false;
}

basic::uint32 BaseShader::get_handle() const
{
    return m_handle;
}

bool BaseShader::is_valid() const
{
    return is_shader_compiled( m_handle );
}

BaseShader *BaseShader::create(const char *file)
{
    basic::String filename = file;
    if( filename.ends_of(".vs") )
    {
        return new BaseShader( GL_VERTEX_SHADER, file );
    }
    else if( filename.ends_of(".fs") )
    {
        return new BaseShader( GL_FRAGMENT_SHADER, file );
    }

    return nullptr;
}

