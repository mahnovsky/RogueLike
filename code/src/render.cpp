#include "render.hpp"
#include "window.hpp"
#include "basic/vector.hpp"
#include "basic/file.hpp"
#include "render_object.hpp"

extern "C"
{
#define GLEW_STATIC
#include <GL/glew.h>
}

#include <GL/gl.h>


class Render : public IRender
{
public:
    bool
    init( int width, int height ) override
    {
        if ( glewInit( ) != GLEW_OK )
        {
            return false;
        }

        const char* gl_version = (const char*)glGetString( GL_VERSION );
        LOG( "gl version %s", gl_version );

        glViewport( 0, 0, width, height );
        glClearColor( 0.0, 0.0, 0.0, 1.0 );

        init_shaders( );        

        return true;
    }

    bool
    init_shader( const char* const data, size_t size, GLuint shader_type, GLuint& out )
    {
        GLuint index = glCreateShader( shader_type );

        GLchar* data_ptr = (GLchar*)data;
        GLint gsize = size;

        glShaderSource( index, 1, &data_ptr, &gsize );

        glCompileShader( index );

        out = index;

        return true;
    }

    bool
    init_shaders( )
    {
        basic::Vector< char > data = basic::get_file_content( "vshader.glsl" );
        if ( data.is_empty() )
        {
            LOG("Failed load vshader source file");

            return false;
        }

        GLuint vshader = 0;
        if ( init_shader( data.get_raw( ), data.get_size( ), GL_VERTEX_SHADER, vshader ) )
        {
            LOG( "vshader %u", vshader );

            check_shader( vshader );
        }
        else
        {
            LOG( "failed vshader %u", vshader );

            return false;
        }

        data.clear( );
        data = basic::get_file_content( "fshader.glsl" );

        if ( data.is_empty() ) 
        {
            LOG("Failed read fshader source file");

            return false;
        }

        GLuint fshader = 0;
        if ( init_shader( data.get_raw( ), data.get_size( ), GL_FRAGMENT_SHADER, fshader ) )
        {
            LOG( "fshader %u", fshader );

            check_shader( fshader );
        }
        else
        {
            LOG( "failed fshader %u", fshader );

            return false;
        }

        m_shader_program = glCreateProgram( );

        glAttachShader( m_shader_program, vshader );
        glAttachShader( m_shader_program, fshader );
        glLinkProgram( m_shader_program );

        check_shader_link( m_shader_program );

        glDeleteShader( vshader );
        glDeleteShader( fshader );

        return true;
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

    void
    draw( const RenderObject* graphic ) override
    {
        glUseProgram( m_shader_program );

        graphic->bind();
        
        glDrawElements( GL_TRIANGLES, graphic->get_element_count( ), GL_UNSIGNED_SHORT, (GLvoid*)0 );

        graphic->unbind();
    }

    void
    draw_begin( ) override
    {
        glClear( GL_COLOR_BUFFER_BIT ); //| GL_DEPTH_BUFFER_BIT );
    }

    void
    draw_end( IWindow* wnd ) override
    {
        wnd->swap_buffers();
    }

private:
    GLuint m_shader_program;
};

IRender*
IRender::create( )
{
    return new Render( );
}
