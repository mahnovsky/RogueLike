#include "shader.hpp"

#include "GL/glew.h"
#include "basic/file.hpp"
#include "config.hpp"

#include "render_common.hpp"

basic::uint32 ShaderProgram::m_current_shader_program;

ShaderProgram::ShaderProgram(GenericObjectManager* manager, const char* file )
    : FileResource( manager, file )
    , m_shader_program( 0 )
    , m_vertex_shader( nullptr )
    , m_fragment_shader( nullptr )
{
}

ShaderProgram::~ShaderProgram( )
{
    SAFE_RELEASE( m_vertex_shader );
    SAFE_RELEASE( m_fragment_shader );

    if ( m_shader_program )
    {
        glDeleteProgram( m_shader_program );
    }
}

bool
ShaderProgram::load( ResourceStorage* storage )
{
    Config* config = storage->get_resorce< Config >( "shaders/programs.conf" );

    if ( config )
    {
        const basic::JsonObject* shader_conf = config->get_values( get_file_name().data( ) );

        ASSERT( shader_conf );

        std::string vertex_file;
        shader_conf->get( "vertex_shader", vertex_file );

        if ( vertex_file.empty( ) )
        {
            return false;
        }

        std::string fragment_file;
        shader_conf->get( "fragment_shader", fragment_file );

        if ( fragment_file.empty( ) )
        {
            return false;
        }

		m_shader_program = glCreateProgram();
        BaseShader* vertex = storage->get_resorce< BaseShader >( vertex_file.c_str( ) );
        BaseShader* fragment = storage->get_resorce< BaseShader >( fragment_file.c_str( ) );

        return init( vertex, fragment );
    }

    return false;
}

basic::uint32 ShaderProgram::get_handle() const
{
	return m_shader_program;
}

void
print_shader_info( GLuint shader )
{
    GLchar infoLog[ 512 ];
    glGetShaderInfoLog( shader, 512, nullptr, infoLog );
    LOG( "Shader info: %s", infoLog );
}

bool
is_shader_compiled( GLuint shader )
{
    GLint success = 0;
    CHECKED_CALL(glGetShaderiv, shader, GL_COMPILE_STATUS, &success );

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

ShaderProgram*
ShaderProgram::create( GenericObjectManager* manager, const char* file )
{
    ShaderProgram* res = NEW_OBJ( ShaderProgram, manager, file );

    return res;
}

void
ShaderProgram::bind( ) const
{
    if ( m_current_shader_program != m_shader_program )
    {
        glUseProgram( m_shader_program );
        m_current_shader_program = m_shader_program;
    }
}

void
ShaderProgram::unbind( ) const
{
    glUseProgram( 0 );
    m_current_shader_program = 0;
}

bool
ShaderProgram::init( BaseShader* vertex, BaseShader* fragment )
{
    if ( vertex && vertex->is_valid( ) && fragment && fragment->is_valid( )
         && vertex->get_tag( ) == GL_VERTEX_SHADER
         && fragment->get_tag( ) == GL_FRAGMENT_SHADER )
    {
        if ( link_program( vertex->get_handle( ), fragment->get_handle( ) ) )
        {
			
            m_vertex_shader = vertex;
            m_fragment_shader = fragment;

            vertex->ref( );
            fragment->ref( );

            return true;
        }
    }
    LOG( "Failed link shader program" );

    return false;
}

bool
ShaderProgram::link_program( basic::uint32 vshader, basic::uint32 fshader )
{
    
    glAttachShader( m_shader_program, vshader );
    glAttachShader( m_shader_program, fshader );
    glLinkProgram( m_shader_program );

	OPENGL_CHECK_FOR_ERRORS();

    return check_shader_link(m_shader_program);
}

static GLuint
compile( std::vector< uint8_t > data, GLenum type )
{
	OPENGL_CHECK_FOR_ERRORS();
    GLuint handle = CHECKED_CALL( glCreateShader, type );
    data.push_back( 0 );
    char* data_ptr = reinterpret_cast< GLchar* >( data.data( ) );
    auto gsize = static_cast< GLint >( data.size( ) );

    CHECKED_CALL(glShaderSource, handle, 1, &data_ptr, &gsize );
    CHECKED_CALL(glCompileShader, handle );

    return handle;
}

BaseShader::BaseShader( GenericObjectManager* manager, basic::uint32 type, const char* file )
    : FileResource( manager, file )
    , m_handle( 0 )
	, m_type(type)
    , m_tag(type)
{
    bool is_valid = type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER;
    ASSERT( is_valid );
}

BaseShader::~BaseShader( )
{
    glDeleteShader( m_handle );
}

bool
BaseShader::load( ResourceStorage* )
{
    if ( m_handle != 0 )
    {
        return true;
    }

    auto file = get_file_name( );
    ASSERT( !file.empty( ) );

    auto data = basic::get_file_content( file.data( ) );

    if ( !data.empty( ) )
    {
        m_handle = compile( data, m_type );

        bool valid = is_valid( );
        if ( !valid )
        {
            print_shader_info( m_handle );
        }
        return valid;
    }

    return false;
}

basic::uint32
BaseShader::get_handle( ) const
{
    return m_handle;
}

bool
BaseShader::is_valid( ) const
{
    return is_shader_compiled( m_handle );
}

BaseShader* BaseShader::create( GenericObjectManager* manager, const char* file )
{
    basic::String filename = file;
    if ( filename.ends_of( ".vs" ) )
    {
        return NEW_OBJ( BaseShader, manager, GL_VERTEX_SHADER, file );
    }
    else if ( filename.ends_of( ".fs" ) )
    {
        return NEW_OBJ( BaseShader, manager, GL_FRAGMENT_SHADER, file );
    }

    return nullptr;
}
